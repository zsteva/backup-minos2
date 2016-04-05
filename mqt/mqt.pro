TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    XMPPClientLib \
    MqtBase \
    RPCTest \
    MqtLogger \
    MqtServer \
    MqtChat \
    MqtTestRotator

RPCTest.depends = MqtUtils TinyXML XMPPLib XMPPClientLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib XMPPClientLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib XMPPClientLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib XMPPClientLib MqtBase
