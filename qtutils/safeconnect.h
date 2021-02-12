//
// Copyright (c) Vladimir Kraus. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for details.
// https://github.com/vladimir-kraus/qtutils
//

//
// Helper functions safeConnect() and safeDisconnect() which can help you catching
// several potential issues related to connecting signals and slots. It checks (assertion
// at runtime in debug mode) success and uniqueness of connection and success of disconnection.
//

#pragma once

#include <QObject>

/*
  * Call safeConnect() to establish a connection in safe and checked way.
  * safeConnect() asserted at runtime (debug mode only) the following:
  * - success of connection
  * - uniqueness of connection
  *
  * Usage:
  * 1) safeConnect(sender, signal, receiver, slot);
  * 2) safeConnect(sender, signal, context, functor);
  *
  * Even with the modern style of connections introduced in Qt5,
  * there are still a few reasons why connection may fail. These are:
  * a) connecting to a non-signal function (can be identified with Clazy)
  * b) connecting to nullptr sender
  * c) connecting to nullptr receiver
  * safeConnect() asserts that none of these occurs at runtime (debug mode only).
  *
  * Of course safeConnect() will catch many more potential problems which can be
  * caused by Qt4-style of connections (but you should not use them any more).
  *
  * Another potential problem with conections are non-unique connections.
  * The connections are by asserted to be unique because there are only extremely
  * rare cases when non-unique connections are desirable and intentional.
  * Qt does not enforce unique connections as default, which is a shame.
  * Multiple connections between the same pair of sender+signal and receiver+slot
  * are hard to find because the application usually seems to work fine.
  * But the performance is worse because some slots may be called twice or more times.
  * Since Qt and C++ is often sufficiently fast, this may remain
  * unnoticed for quite some time. safeConnect() will check this for you.
  *
  * safeConnect() intentionally does not support 3-argument static overloads of
  * connect(), because these are used for lambdas and these inherently are less safe.
  * The reason is that lambdas often capture certain object and if this object
  * gets deleted while the lambda being still connected, this can (and will)
  * lead to undefined behaviour. Therefore this 4-argument overload
  * will force the user to use context parameter when connecting to
  * a lambda as in connect(sender, signal, context, functor) overload.
  *
  * Lambdas are also always invoked in the same thread where the signal was emitted,
  * despite the fact that they capture and call methods of a worker object
  * living in another thread. This is however not possible to check with safeConnect().
  *
  * It intentionally does not support non-static overload called in form of
  * receiver.connect(sender, signal, slot) which should be avoided
  * because the order of sender/receiver is counterintuitive.
  *
  * Note that connection uniqueness cannot be enforced for lambdas.
  * Keep in mind that connections to lambdas are always cumbersome.
  */
template <typename Sender, typename Signal, typename Receiver, typename Slot>
void safeConnect(Sender sender, Signal signal, Receiver receiver, Slot slot)
{
    bool success = QObject::connect(sender, signal, receiver, slot, Qt::UniqueConnection);
    Q_ASSERT(success);
    Q_UNUSED(success)
}

/*
  * Call safeDisconnect() to remove connection in a safe and checked way.
  * The success of disconnection is asserted at runtime (in debug mode).
  *
  * Usage:
  * safeDisconnect(sender, signal, receiver, slot);
  *
  * If you are not sure whether the connection is established,
  * then you need to call QObject::disconnect() instead of safeDisconnect().
  * This would however be probably a sign of a brittle, error prone design.
  *
  * Note that disconnecting from lambdas is not possible.
  */
template <typename Sender, typename Signal, typename Receiver, typename Slot>
void safeDisconnect(Sender sender, Signal signal, Receiver receiver, Slot slot)
{
    bool success = QObject::disconnect(sender, signal, receiver, slot);
    Q_ASSERT(success);
    Q_UNUSED(success);
}
