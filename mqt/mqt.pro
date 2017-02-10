TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    MqtBase \
    RPCTest \
    MqtLogger \
    MqtServer \
    mqtKeyer

win32 {
    SUBDIRS += \
    MqtChat \
    MqtTestRotator \
    mqtControl
}

RPCTest.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtTestRotator.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtControl.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtKeyer.depends = MqtUtils TinyXML XMPPLib MqtBase
