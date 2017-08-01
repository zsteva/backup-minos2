TEMPLATE = subdirs

SUBDIRS += \
    MqtUtils \
    TinyXML \
    XMPPLib \
    MqtBase \
    RPCTest \
    MqtAppStarter \
    MqtControl \
    MqtChat \
    MqtKeyer \
    MqtLogger \
    MqtServer \
	MqtRotator \
	MqtBandmap \
	MqtMonitor \
	MqtRigControl<<<<<<< .mine
    MqtCluster \
    MqtRigControl

=======
    MqtRotator \
    MqtBandmap \
    MqtMonitor
>>>>>>> .theirs

greaterThan(QT_MAJOR_VERSION, 4) : greaterThan(QT_MINOR_VERSION, 6) {
   SUBDIRS += mqtKeyerTest
}


RPCTest.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtAppStarter.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtBandMap.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtChat.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtControl.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtKeyer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtLogger.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtServer.depends = MqtUtils TinyXML XMPPLib MqtBase
MqtRotator.depends = MqtUtils TinyXML XMPPLib MqtBase
