TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    MqtBase \
    RPCTest \
    MqtLogger \
    MqtServer \
    MqtChat \
    MqtTestRotator \
    mqtControl \
    mqtKeyer

RPCTest.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtControl.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtKeyer.depends = MqtUtils TinyXML XMPPLib MqtBase
