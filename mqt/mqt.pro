TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    MqtBase \
    RPCTest \
    MqtLogger \
    MqtServer \
    MqtRotator \
    MqtBandmap \
    MqtTestrpc1 \
    MqtTestrpc2


!unix {
    SUBDIRS += \
    MqtChat \
    MqtRotator \
    mqtControl \
    mqtKeyer
}

RPCTest.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtRotator.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtControl.depends = MqtUtils TinyXML XMPPLib MqtBase
mqtKeyer.depends = MqtUtils TinyXML XMPPLib MqtBase
