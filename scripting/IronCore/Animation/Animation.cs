﻿using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

namespace Iron
{
    /// <summary>
    /// Loaded animation clip
    /// </summary>
    public class Animation : Resource
    {
        internal Animation(uint id) : base(id) { }

        /// <summary>
        /// Animation name
        /// </summary>
        public string Name
        {
            get => GetName_Internal(ID);
            set => SetName_Internal(ID, value);
        }

        /// <summary>
        /// Is animation looped
        /// </summary>
        public bool Loop
        {
            get => GetLoop_Internal(ID);
            set => SetLoop_Internal(ID, value);
        }

        /// <summary>
        /// Animation length in seconds
        /// </summary>
        public float Length => GetLength_Internal(ID);

        /// <summary>
        /// Adds empty frame at the end of animation
        /// </summary>
        /// <remarks>This can be useful for explosion-like animations to not leave anything to render after played</remarks>
        public void EndWithEmptyFrame() => EndWithEmptyFrame_Internal(ID);

        /// <summary>
        /// Creates animation from sprite sheet
        /// </summary>
        /// <param name="sprite">Sprite that was set as sprite sheet</param>
        /// <param name="length">Desired animation length</param>
        /// <returns>New animation or null if creation was not successful</returns>
        /// <remarks>Sprites will be put evenly on the timeline depending on length</remarks>
        public static Animation FromSpriteSheet(Sprite sprite, float length)
        {
            uint animationID = FromSpriteSheet_Internal(sprite.ID, length);
            
            return animationID == NULL_RESOURCE_ID ? null : new Animation(animationID);
        }

        /// <summary>
        /// Creates animation from multiple sprites
        /// </summary>
        /// <param name="sprites">List of sprites</param>
        /// <param name="length">Desired animation length</param>
        /// <returns>New animation or null if creation was not successful</returns>
        /// <remarks>Sprites will be put evenly on the timeline depending on length</remarks>
        public static Animation FromSprites(IEnumerable<Sprite> sprites, float length)
        {
            uint animationID = FromSprites_Internal(sprites.Select(s => s?.ID ?? Resource.NULL_RESOURCE_ID).ToArray(), length);
            
            return animationID == NULL_RESOURCE_ID ? null : new Animation(animationID);
        }

        public override string ToString()
        {
            return $"Animation {Name}";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint FromSpriteSheet_Internal(uint spriteID, float length);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint FromSprites_Internal(uint[] spritesIDs, float length);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetName_Internal(uint animationID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetName_Internal(uint animationID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool GetLoop_Internal(uint animationID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetLoop_Internal(uint animationID, bool loop);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetLength_Internal(uint animationID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float EndWithEmptyFrame_Internal(uint animationID);
    }
}