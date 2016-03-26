#ifndef ROTATORRPCOBJ_H
#define ROTATORRPCOBJ_H
/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
enum RotateDirection :int {eRotateLeft, eRotateDirect, eRotateRight, eRotateStop};
class RPCRotatorClient: public MinosRPCClient
{
   public:
      RPCRotatorClient( TRPCFunctor *cb ) : MinosRPCClient( "Minos:Rotator", cb )
      {}
      ~RPCRotatorClient()
      {}
      virtual RPCRotatorClient *makeObj()
      {
         return new RPCRotatorClient( callback );
      }
};
class RPCRotatorServer: public MinosRPCServer
{
   public:
      RPCRotatorServer( TRPCFunctor *cb ) : MinosRPCServer( "Minos:Rotator", cb )
      {}
      ~RPCRotatorServer()
      {}
      virtual RPCRotatorServer *makeObj()
      {
         return new RPCRotatorServer( callback );
      }

};

#endif // ROTATORRPCOBJ_H
