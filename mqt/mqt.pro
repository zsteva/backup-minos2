TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    MqtBase \
    RPCTest \
    mqtAppStarter \
    mqtControl \
    MqtChat \
    mqtKeyer \
    MqtLogger \
    MqtServer \
    MqtRotator \
    MqtBandmap


RPCTest.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtAppStarter.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtBandMap.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtControl.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtKeyer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtRotator.depends = MqtUtils TinyXML XMPPLib MqtBase
