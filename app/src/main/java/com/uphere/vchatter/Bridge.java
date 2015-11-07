package com.uphere.vchatter;

public class Bridge
{
    public static native void registerJRef( int k, Object v );

    public static native void onFrameHS( long frameTimeNanos );
}
