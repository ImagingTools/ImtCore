#pragma once


// std includes
#include <cstdint>

// Qt includes
#include <QtCore/QList>


namespace imtdev
{


class IUsbDeviceIdProvider
{

public:
  struct UsbDeviceId
  {
    uint16_t vid;
    uint16_t pid;

    bool operator<(const IUsbDeviceIdProvider::UsbDeviceId& other) const
    {
      return vid == other.vid ? pid < other.pid : vid < other.vid;
    }

    bool operator==(const IUsbDeviceIdProvider::UsbDeviceId& other) const
    {
      return vid == other.vid && pid == other.pid;
    }

    bool operator!=(const IUsbDeviceIdProvider::UsbDeviceId& other) const
    {
      return !operator==(other);
    }
  };

  typedef QList<UsbDeviceId> UsbDeviceIdList;

  virtual const IUsbDeviceIdProvider::UsbDeviceIdList& GetUsbDeviceIdList() const = 0;
};


} // namespace imtdev

Q_DECLARE_METATYPE(imtdev::IUsbDeviceIdProvider::UsbDeviceId);
