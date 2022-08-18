// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// silentcall.h
// call a slot without sending a signal
// ---------------------------------------------------------------------------
// Copyright (C) 2020 by t2ft - Thomas Thanner
// Waldstraße 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2020-3-23  tt  Initial version created
// ---------------------------------------------------------------------------
// SilentCall is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
// SilentCall is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with SilentCall.  If not, see <https://www.gnu.org/licenses/>.
// ***************************************************************************
#ifndef SILENTCALL_H
#define SILENTCALL_H

template<class T>
class SignalBlockerCallProxy
{
    T * const o;
public:
    explicit SignalBlockerCallProxy( T * oo )
      : o(oo)
    {
    }
    T* operator->()
    {
         return o;
    }
    ~SignalBlockerCallProxy()
    {
        if(o)
            o->blockSignals(false);
    }
};

template<class T>
class SignalBlocker
{
    T * const o;
public:
    explicit SignalBlocker( T * oo )
      : o(oo)
    {
    }
    SignalBlockerCallProxy<T> operator->()
    {
         if (o)
             o->blockSignals( true );
         return SignalBlockerCallProxy<T>(o);
    }
    ~SignalBlocker() {}
};

template<class T>
SignalBlocker<T> SilentCall(T* o)
{
    return SignalBlocker<T>(o);
}

#endif // SILENTCALL_H
