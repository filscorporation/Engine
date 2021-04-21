﻿using System.Runtime.CompilerServices;

namespace Iron
{
    /// <summary>
    /// Components that will get rendered in the scene
    /// </summary>
    public class SpriteRenderer : Component
    {
        /// <summary>
        /// Sprite to render
        /// </summary>
        public Sprite Sprite
        {
            get => new Sprite(GetSprite_Internal(Entity.ID));
            set => SetSprite_Internal(Entity.ID, value?.ID ?? Resource.NULL_RESOURCE_ID);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint GetSprite_Internal(uint entityID);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetSprite_Internal(uint entityID, uint spriteID);
    }
}