# Migration Guide: IJobTicket Status Separation

## Overview

This guide helps developers migrate code from the old IJobTicket-based status management to the new separated IJobStatus architecture.

## Quick Summary

**What Changed:**
- Job status and progress are now managed separately from job configuration
- Use `IJobQueueManager` methods instead of `IJobTicket` for status operations
- `IJobTicket` interface still has status methods for backward compatibility (deprecated)

**Why:**
- Better performance for status queries
- Cleaner separation of concerns
- Foundation for future optimizations

## Migration Patterns

### Pattern 1: Simple Status Query

#### Before (Deprecated)
```cpp
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
if (ticket.IsValid()) {
    ProcessingStatus status = ticket->GetProcessingStatus();
    // use status
}
```

#### After (Recommended)
```cpp
// Direct query through IJobQueueManager
ProcessingStatus status = jobQueueManager->GetProcessingStatus(jobId);
// use status
```

**Benefits:**
- No need to load full job configuration
- Faster query (map lookup vs deserialization)
- Clearer intent

---

### Pattern 2: Status Update

#### Before (Deprecated)
```cpp
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
if (ticket.IsValid()) {
    ticket->SetProcessingStatus(PS_RUNNING);
    SaveJobTicket(jobId, ticket); // Updates entire ticket
}
```

#### After (Recommended)
```cpp
// Direct update through IJobQueueManager
jobQueueManager->SetProcessingStatus(jobId, PS_RUNNING);
// Status automatically persisted separately
```

**Benefits:**
- No need to save entire job configuration
- Reduced I/O
- Automatic change notification

---

### Pattern 3: Progress Update

#### Before (Deprecated)
```cpp
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
if (ticket.IsValid()) {
    ticket->SetProgress(0.75);
    SaveJobTicket(jobId, ticket);
}
```

#### After (Recommended)
```cpp
jobQueueManager->SetProgress(jobId, 0.75);
```

**Benefits:**
- Much faster for frequent progress updates
- No serialization overhead
- Cleaner code

---

### Pattern 4: Combined Status and Progress Query

#### Before (Deprecated)
```cpp
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
if (ticket.IsValid()) {
    ProcessingStatus status = ticket->GetProcessingStatus();
    double progress = ticket->GetProgress();
    
    // Update UI
    UpdateStatusDisplay(status, progress);
}
```

#### After (Recommended)
```cpp
ProcessingStatus status = jobQueueManager->GetProcessingStatus(jobId);
double progress = jobQueueManager->GetProgress(jobId);

// Update UI
UpdateStatusDisplay(status, progress);
```

**Benefits:**
- Two lightweight queries instead of one heavy deserialization
- Better for high-frequency polling scenarios

---

### Pattern 5: Batch Status Queries

#### Before (Inefficient)
```cpp
QList<JobStatusInfo> statuses;
for (const QByteArray& jobId : jobIds) {
    IJobTicketSharedPtr ticket = GetJobTicket(jobId);
    if (ticket.IsValid()) {
        JobStatusInfo info;
        info.jobId = jobId;
        info.status = ticket->GetProcessingStatus();
        info.progress = ticket->GetProgress();
        statuses.append(info);
    }
}
```

#### After (Efficient)
```cpp
QList<JobStatusInfo> statuses;
for (const QByteArray& jobId : jobIds) {
    JobStatusInfo info;
    info.jobId = jobId;
    info.status = jobQueueManager->GetProcessingStatus(jobId);
    info.progress = jobQueueManager->GetProgress(jobId);
    statuses.append(info);
}
```

**Benefits:**
- Significantly faster for large job lists
- Reduced memory usage
- Better scalability

---

### Pattern 6: Status Change Notifications (Unchanged)

```cpp
// Subscribing to changes - no changes needed
connect(jobQueueManager, &IJobQueueManager::Changed,
        this, &MyClass::OnJobQueueChanged);

void MyClass::OnJobQueueChanged(const istd::IChangeable::ChangeSet& changes) {
    if (changes.HasChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED)) {
        QVariant value = changes.GetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED);
        auto info = value.value<IJobQueueManager::JobStatusInfo>();
        
        // Handle status change
        HandleStatusChange(info.elementId, info.status);
    }
    
    if (changes.HasChangeInfo(IJobQueueManager::CN_JOB_PROGRESS_CHANGED)) {
        QVariant value = changes.GetChangeInfo(IJobQueueManager::CN_JOB_PROGRESS_CHANGED);
        auto info = value.value<IJobQueueManager::JobProgressInfo>();
        
        // Handle progress change
        HandleProgressChange(info.elementId, info.progress);
    }
}
```

**No Changes Required:**
- Change notification mechanism unchanged
- Same notification types
- Same data structures

---

### Pattern 7: Job Configuration (Unchanged)

```cpp
// Reading job configuration - no changes needed
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
if (ticket.IsValid()) {
    QByteArray typeId = ticket->GetTypeId();
    QString jobName = ticket->GetJobName();
    ParamsPtr params = ticket->GetParams();
    const IJobOutput* results = ticket->GetResults();
    
    // Use configuration
}
```

**No Changes Required:**
- Job configuration access unchanged
- Use IJobTicket for configuration data
- Status queries should use IJobQueueManager

---

## Component-Specific Migration

### For Job Processors/Workers

```cpp
class MyJobProcessor : public IJobProcessor {
public:
    virtual bool Process(const QByteArray& jobId, IJobQueueManager* queueManager) override {
        // Get job configuration
        IJobTicketSharedPtr ticket = GetJobTicket(jobId);
        ParamsPtr params = ticket->GetParams();
        
        // Update status (new way)
        queueManager->SetProcessingStatus(jobId, PS_RUNNING);
        queueManager->SetProgress(jobId, 0.0);
        
        // Do work
        for (int i = 0; i < 100; i++) {
            DoWorkStep(i);
            
            // Update progress (new way - more efficient)
            queueManager->SetProgress(jobId, i / 100.0);
        }
        
        // Mark complete
        queueManager->SetProcessingStatus(jobId, PS_FINISHED);
        
        return true;
    }
};
```

---

### For UI Components

```cpp
class JobMonitorWidget : public QWidget {
private:
    void UpdateJobDisplay(const QByteArray& jobId) {
        // Get static configuration (cached)
        if (!m_cachedTickets.contains(jobId)) {
            m_cachedTickets[jobId] = GetJobTicket(jobId);
        }
        IJobTicketSharedPtr ticket = m_cachedTickets[jobId];
        
        // Get dynamic status (always fresh)
        ProcessingStatus status = m_jobQueueManager->GetProcessingStatus(jobId);
        double progress = m_jobQueueManager->GetProgress(jobId);
        
        // Update UI
        m_nameLabel->setText(ticket->GetJobName());
        m_statusLabel->setText(StatusToString(status));
        m_progressBar->setValue(progress * 100);
    }
    
private:
    IJobQueueManager* m_jobQueueManager;
    QHash<QByteArray, IJobTicketSharedPtr> m_cachedTickets;
};
```

**Key Points:**
- Cache job configuration (rarely changes)
- Query status/progress directly (changes frequently)
- Better performance for UI updates

---

### For Database/Persistence Layer

```cpp
// Example: Update database schema
// Old schema (mixed concerns):
// CREATE TABLE JobTickets (
//     JobId BLOB PRIMARY KEY,
//     TypeId BLOB,
//     JobName TEXT,
//     Status INTEGER,      -- Mixed with configuration
//     Progress REAL,       -- Mixed with configuration
//     Configuration BLOB
// );

// New schema (separated concerns):
// CREATE TABLE JobTickets (
//     JobId BLOB PRIMARY KEY,
//     TypeId BLOB,
//     JobName TEXT,
//     Configuration BLOB
// );
//
// CREATE TABLE JobStatus (
//     JobId BLOB PRIMARY KEY,
//     Status INTEGER,
//     Progress REAL,
//     LastUpdated TIMESTAMP,
//     FOREIGN KEY (JobId) REFERENCES JobTickets(JobId)
// );
// CREATE INDEX idx_job_status_status ON JobStatus(Status);
```

**Migration Strategy:**
1. Add new JobStatus table
2. Migrate existing status data
3. Update queries to use separate tables
4. Remove status columns from JobTickets table (after verification)

---

## Testing Your Migration

### Unit Test Template

```cpp
class JobStatusMigrationTest : public QObject {
    Q_OBJECT
    
private slots:
    void testStatusQueryPerformance() {
        // Create test jobs
        QList<QByteArray> jobIds;
        for (int i = 0; i < 1000; i++) {
            jobIds.append(CreateTestJob());
        }
        
        // Test new method
        QElapsedTimer timer;
        timer.start();
        for (const QByteArray& jobId : jobIds) {
            ProcessingStatus status = m_queueManager->GetProcessingStatus(jobId);
            Q_UNUSED(status);
        }
        qint64 newTime = timer.elapsed();
        
        qDebug() << "New method:" << newTime << "ms for 1000 queries";
        
        // Should be much faster than old method
        QVERIFY(newTime < 100); // Less than 100ms for 1000 queries
    }
    
    void testStatusUpdate() {
        QByteArray jobId = CreateTestJob();
        
        // Update status
        m_queueManager->SetProcessingStatus(jobId, PS_RUNNING);
        
        // Verify
        ProcessingStatus status = m_queueManager->GetProcessingStatus(jobId);
        QCOMPARE(status, PS_RUNNING);
    }
    
    void testProgressUpdate() {
        QByteArray jobId = CreateTestJob();
        
        // Update progress
        m_queueManager->SetProgress(jobId, 0.75);
        
        // Verify
        double progress = m_queueManager->GetProgress(jobId);
        QVERIFY(qFuzzyCompare(progress, 0.75));
    }
};
```

---

## Common Pitfalls

### Pitfall 1: Still Loading Full Ticket for Status

❌ **Wrong:**
```cpp
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
ProcessingStatus status = ticket->GetProcessingStatus(); // Deprecated!
```

✅ **Correct:**
```cpp
ProcessingStatus status = jobQueueManager->GetProcessingStatus(jobId);
```

---

### Pitfall 2: Saving Ticket After Status Update

❌ **Wrong:**
```cpp
ticket->SetProcessingStatus(PS_RUNNING);
SaveJobTicket(jobId, ticket); // Unnecessary!
```

✅ **Correct:**
```cpp
jobQueueManager->SetProcessingStatus(jobId, PS_RUNNING);
// Automatically handled
```

---

### Pitfall 3: Caching Status in Ticket Object

❌ **Wrong:**
```cpp
// Caching ticket for status queries
m_cachedTicket = GetJobTicket(jobId);
// Later...
ProcessingStatus status = m_cachedTicket->GetProcessingStatus(); // Stale!
```

✅ **Correct:**
```cpp
// Cache ticket for configuration
m_cachedTicket = GetJobTicket(jobId);
QString name = m_cachedTicket->GetJobName(); // OK

// Always query status directly
ProcessingStatus status = jobQueueManager->GetProcessingStatus(jobId); // Fresh
```

---

## Performance Expectations

### Before Migration
- Status query: ~10-50ms (includes deserialization)
- Status update: ~50-100ms (includes serialization)
- Batch query (100 jobs): ~1-5 seconds

### After Migration
- Status query: <1ms (map lookup)
- Status update: <5ms (map update + notification)
- Batch query (100 jobs): ~50-100ms

**Expected Improvement:** 10-50x faster for status operations

---

## Rollback Plan

If issues arise, the old behavior is still available:

```cpp
// Temporarily revert to old pattern
IJobTicketSharedPtr ticket = GetJobTicket(jobId);
ProcessingStatus status = ticket->GetProcessingStatus();
ticket->SetProcessingStatus(newStatus);
SaveJobTicket(jobId, ticket);
```

However, this is not recommended long-term as it loses the performance benefits.

---

## Support and Questions

If you encounter issues during migration:

1. Check this migration guide
2. Review the architectural documentation: `ARCHITECTURE_JOB_STATUS_SEPARATION.md`
3. Look at example code in updated components
4. Contact the architecture team

---

## Checklist for Migration

- [ ] Identify all IJobTicket status/progress usage in your code
- [ ] Replace with IJobQueueManager calls
- [ ] Update unit tests
- [ ] Verify change notifications still work
- [ ] Performance test critical paths
- [ ] Update documentation
- [ ] Code review
- [ ] Deploy to test environment
- [ ] Monitor performance metrics
- [ ] Deploy to production

---

## Version History

- **v1.0** (2026-01-25): Initial migration guide
- **Author**: Architecture Team
