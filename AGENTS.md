# Coding Agent Guidelines

These rules apply across the organization unless a repository-specific `AGENTS.md` or other local instructions override them.

## 1. Follow local instructions first

- Always follow repository-specific instructions, coding standards, and workflows.
- If local guidance conflicts with this file, the more specific repository-level guidance wins.

## 2. Work minimally and safely

- Make the smallest change that fully solves the task.
- Avoid unrelated refactors, formatting-only changes, or scope creep.
- Keep pull requests focused and easy to review.
- Preserve backward compatibility unless a breaking change is explicitly required.

## 3. Use the right source of truth

- If the repository depends on parent repositories, shared libraries, or upstream services, check them before guessing behavior.
- Prefer upstream or shared repositories as the source of truth for public interfaces, shared data models, and cross-repository behavior.
- If required context is missing, state what is missing and proceed only with the information available.

## 4. Follow organization-wide structural standards

- Use `ImagingTools/Acf` as the formatting reference repository.
- Use `ImagingTools/ImtCore` as the directory structure reference repository.
- QML UIs must follow the structure and style used in `ImagingTools/ImtCore`.
- Component tests must be organized like in `ImagingTools/Acf`.
- Data model primitives must be implemented like in `ImagingTools/Acf`.

## 5. Keep documentation in /Docs

- Store all documentation, including `.md` files, under `/Docs`.
- Organize documentation into appropriate subdirectories within `/Docs`.
- Do not place documentation at the repository root unless there is a documented exception.

## 6. Maintain quality

- Understand the existing code path before changing it.
- Update or add tests when behavior changes.
- Check impacted call sites, configs, and data formats.
- Do not leave known breakage unresolved.

## 7. Be careful with security and dependencies

- Never add secrets, tokens, credentials, or personal data to code, logs, tests, or documentation.
- Treat authentication, authorization, and access-control changes as sensitive.
- Do not add new dependencies unless there is a strong justification.

## 8. Communicate clearly

- State assumptions clearly when requirements are ambiguous.
- Ask for clarification instead of guessing when needed.
- Summarize what changed, why it changed, and how it was verified.

## 9. Preferred output from the agent

When completing a task, the agent should provide:

- A short summary of the change
- The files changed
- Tests run or why tests were not run
- Any remaining risks or follow-up work
