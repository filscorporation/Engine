﻿using System;
using System.Runtime.InteropServices;

namespace Iron
{
    public static class EngineCalls
    {
        private static void ComponentOnCreate(IntPtr componentPtr)
        {
            ((Component)GCHandle.FromIntPtr(componentPtr).Target).OnCreate();
        }
        
        private static void ComponentOnUpdate(IntPtr componentPtr)
        {
            ((Component)GCHandle.FromIntPtr(componentPtr).Target).OnUpdate();
        }
        
        private static void ComponentOnLateUpdate(IntPtr componentPtr)
        {
            ((Component)GCHandle.FromIntPtr(componentPtr).Target).OnLateUpdate();
        }
        
        private static void ComponentOnFixedUpdate(IntPtr componentPtr)
        {
            ((Component)GCHandle.FromIntPtr(componentPtr).Target).OnFixedUpdate();
        }
    }
}