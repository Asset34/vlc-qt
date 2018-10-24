#ifndef VLCQT_MEDIA_DISCOVERER_H_
#define VLCQT_MEDIA_DISCOVERER_H_

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QString>

#include "SharedExportCore.h"

class VlcInstance;
class VlcMedia;
class VlcMediaList;

struct libvlc_event_t;
struct libvlc_event_manager_t;
struct libvlc_media_t;
struct libvlc_media_list_t;
struct libvlc_media_discoverer_t;

class VLCQT_CORE_EXPORT VlcMediaDiscoverer : public QObject
{
    Q_OBJECT

public:
    explicit VlcMediaDiscoverer(const QString &serviceName,
                                VlcInstance *instance);
    virtual ~VlcMediaDiscoverer();

    libvlc_media_discoverer_t *core();

    VlcMediaList *getList() const;

    bool isRunning() const;

signals:
    void started();
    void ended();
    void mediaDiscovered();

private:
    static void libvlc_callback(const libvlc_event_t *event,
                                void *data);
    static void libvlc_mediaList_callback(const libvlc_event_t *event,
                                          void *data);

    void createCoreConnections();
    void removeCoreConnections();

    void createMediaListConnections();
    void removeMediaListConnections();

    libvlc_media_discoverer_t *_vlcMediaDiscoverer;
    libvlc_media_list_t *_vlcMediaList;
    libvlc_event_manager_t *_vlcEvents;
    libvlc_event_manager_t *_vlcMediaListEvents;

    VlcMediaList *_mediaList;
};

#endif // VLCQT_MEDIA_DISCOVERER_H_
