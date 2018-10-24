#include <vlc/vlc.h>

#include "core/Error.h"
#include "core/Instance.h"
#include "core/Media.h"
#include "core/MediaList.h"
#include "core/MediaDiscoverer.h"

VlcMediaDiscoverer::VlcMediaDiscoverer(const QString &serviceName,
                                       VlcInstance *instance)
    : QObject(instance)
{
    _vlcMediaDiscoverer = libvlc_media_discoverer_new_from_name(instance->core(),
                                                                serviceName.toUtf8().data());
    _vlcMediaList = libvlc_media_discoverer_media_list(_vlcMediaDiscoverer);

    _mediaList = new VlcMediaList(instance);

    _vlcEvents = libvlc_media_discoverer_event_manager(_vlcMediaDiscoverer);
    _vlcMediaListEvents = libvlc_media_list_event_manager(_vlcMediaList);

    createCoreConnections();
    createMediaListConnections();

    VlcError::showErrmsg();
}

VlcMediaDiscoverer::~VlcMediaDiscoverer()
{
    removeCoreConnections();
    removeMediaListConnections();

    libvlc_media_discoverer_release(_vlcMediaDiscoverer);

    VlcError::showErrmsg();
}

libvlc_media_discoverer_t *VlcMediaDiscoverer::core()
{
    return _vlcMediaDiscoverer;
}

VlcMediaList *VlcMediaDiscoverer::getList() const
{
    return _mediaList;
}

bool VlcMediaDiscoverer::isRunning() const
{
    int isRunning = libvlc_media_discoverer_is_running(_vlcMediaDiscoverer);

    VlcError::showErrmsg();

    return isRunning;
}

void VlcMediaDiscoverer::libvlc_callback(const libvlc_event_t *event, void *data)
{
    VlcMediaDiscoverer *core = static_cast<VlcMediaDiscoverer*>(data);

    switch (event->type) {
    case libvlc_MediaDiscovererStarted:
        emit core->started();
        break;
    case libvlc_MediaDiscovererEnded:
        emit core->ended();
        break;
    default:
        break;
    }
}

void VlcMediaDiscoverer::libvlc_mediaList_callback(const libvlc_event_t *event, void *data)
{
    VlcMediaDiscoverer *core = static_cast<VlcMediaDiscoverer*>(data);

    switch (event->type) {
    case libvlc_MediaListItemAdded:
        core->_mediaList->addMedia(new VlcMedia(event->u.media_list_item_added.item));

        emit core->mediaDiscovered();
        break;
    case libvlc_MediaListItemDeleted:
        core->_mediaList->removeMedia(event->u.media_list_item_deleted.index);
        break;
    default:
        break;
    }
}

void VlcMediaDiscoverer::createCoreConnections()
{
    QList<libvlc_event_e> list;
    list << libvlc_MediaDiscovererStarted
         << libvlc_MediaDiscovererEnded;

    foreach (const libvlc_event_e &event, list) {
        libvlc_event_attach(_vlcEvents, event, libvlc_callback, this);
    }
}

void VlcMediaDiscoverer::removeCoreConnections()
{
    QList<libvlc_event_e> list;
    list << libvlc_MediaDiscovererStarted
         << libvlc_MediaDiscovererEnded;

    foreach (const libvlc_event_e &event, list) {
        libvlc_event_detach(_vlcEvents, event, libvlc_callback, this);
    }
}

void VlcMediaDiscoverer::createMediaListConnections()
{
    QList<libvlc_event_e> list;
    list << libvlc_MediaListItemAdded
         << libvlc_MediaListItemDeleted;

    foreach (const libvlc_event_e &event, list) {
        libvlc_event_attach(_vlcMediaListEvents, event, libvlc_mediaList_callback, this);
    }
}

void VlcMediaDiscoverer::removeMediaListConnections()
{    QList<libvlc_event_e> list;
     list << libvlc_MediaListItemAdded
          << libvlc_MediaListItemDeleted;

     foreach (const libvlc_event_e &event, list) {
         libvlc_event_detach(_vlcMediaListEvents, event, libvlc_mediaList_callback, this);
     }
}
