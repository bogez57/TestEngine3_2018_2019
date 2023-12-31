#ifndef FIGHTER_INCLUDE
#define FIGHTER_INCLUDE

#include "2d_skeleton.h"
#include "2d_animation.h"
#include "2d_collision_detection.h"
#include "my_math.h"

struct Fighter
{
    Fighter() = default;
    
    Transform world;
    f32 height {};
    Skeleton skel;
    AnimationQueue animQueue;
    AnimationData animData;
    HitBox hitBox;
    HurtBox hurtBox;
};

void InitFighter(Fighter&& fighter, AnimationData animData, Skeleton skel, f32 fighterHeight, HurtBox defaultHurtBox,v2 worldPos, b flipX);

#endif

#ifdef FIGHTER_IMPL

void InitFighter(Fighter&& fighter, AnimationData animData, Skeleton skel, f32 fighterHeight, HurtBox defaultHurtBox, v2 worldPos, b flipX = false)
{
    fighter.skel = skel;//Deep copy needed??
    fighter.animData = animData;
    fighter.world = { worldPos, 0.0f, {1.0f, 1.0f} };
    fighter.height = fighterHeight;
    fighter.hurtBox = defaultHurtBox;
    
    f32 scaleFactorForHeightAdjustment {};
    { //Change fighter height
        f32 aspectRatio = fighter.skel.height / fighter.skel.width;
        scaleFactorForHeightAdjustment = fighter.height / fighter.skel.height;
        
        //New fighter height
        fighter.skel.height = fighter.height;
        fighter.skel.width = aspectRatio * fighter.height;
        
        for (i32 boneIndex {}; boneIndex < bgz::Size(&fighter.skel.bones); ++boneIndex)
        {
            fighter.skel.bones[boneIndex].worldSpace.scale = fighter.world.scale;
            fighter.skel.bones[boneIndex].parentBoneSpace.translation.x *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
            fighter.skel.bones[boneIndex].parentBoneSpace.translation.y *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
            fighter.skel.bones[boneIndex].initialPos_parentBoneSpace.x *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
            fighter.skel.bones[boneIndex].initialPos_parentBoneSpace.y *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
            fighter.skel.bones[boneIndex].length *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
        };
        
        for (i32 slotI {}; slotI < bgz::Size(&fighter.skel.slots); ++slotI)
        {
            fighter.skel.slots[slotI].regionAttachment.height *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
            fighter.skel.slots[slotI].regionAttachment.width *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
            fighter.skel.slots[slotI].regionAttachment.parentBoneSpace.translation.x *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
            fighter.skel.slots[slotI].regionAttachment.parentBoneSpace.translation.y *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
        };
        
        for (i32 animIndex {}; animIndex < bgz::Size(&fighter.animData.animMap.animations); ++animIndex)
        {
            Animation* anim = (Animation*)&fighter.animData.animMap.animations[animIndex];
            
            if (anim->name)
            {
                anim = GetAnimation(fighter.animData.animMap, anim->name);
                for (i32 hitBoxIndex {}; hitBoxIndex < anim->hitBoxes.length; ++hitBoxIndex)
                {
                    anim->hitBoxes[hitBoxIndex].size.width *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
                    anim->hitBoxes[hitBoxIndex].size.height *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
                    anim->hitBoxes[hitBoxIndex].worldPosOffset.x *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
                    anim->hitBoxes[hitBoxIndex].worldPosOffset.y *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
                };
            }
        };
    };
    
    { //Adjust animations to new height standards
        //TODO: Very stupid, move out or change as I'm currently iterating over ALL keyInfos for which there are a lot in my current hashMap_Str class
        for (i32 animIndex {}; animIndex < bgz::Size(&fighter.animData.animMap.animations); ++animIndex)
        {
            Animation anim = fighter.animData.animMap.animations[animIndex];
            
            if (anim.name)
            {
                for (i32 boneIndex {}; boneIndex < bgz::Size(&fighter.skel.bones); ++boneIndex)
                {
                    TranslationTimeline* translationTimeline = &anim.boneTranslationTimelines[boneIndex];
                    
                    if (translationTimeline->exists)
                    {
                        for (i32 keyFrameIndex {}; keyFrameIndex < translationTimeline->timesCount; ++keyFrameIndex)
                        {
                            translationTimeline->translations[keyFrameIndex].x *= (scaleFactorForHeightAdjustment * fighter.world.scale.x);
                            translationTimeline->translations[keyFrameIndex].y *= (scaleFactorForHeightAdjustment * fighter.world.scale.y);
                        };
                    };
                };
            };
        };
    };
    
    //Flip skeleton bones world positions/rotations
    if (flipX)
    {
        for (i32 i {}; i < bgz::Size(&fighter.skel.bones); ++i)
        {
            if (fighter.skel.bones[i].isRoot)
                fighter.skel.bones[i].parentBoneSpace.scale.x = -1.0f;
            else
                fighter.skel.bones[i].worldSpace.scale.y = -1.0f;
        };
        
        UpdateSkeletonBoneWorldTransforms($(fighter.skel), fighter.world.translation);
        
        for (i32 animIndex {}; animIndex < bgz::Size(&fighter.animData.animMap.animations); ++animIndex)
        {
            Animation* anim = &fighter.animData.animMap.animations[animIndex];
            
            if (anim->name)
            {
                anim = GetAnimation(fighter.animData.animMap, anim->name);
                for (i32 hitBoxIndex {}; hitBoxIndex < anim->hitBoxes.length; ++hitBoxIndex)
                {
                    anim->hitBoxes[hitBoxIndex].worldPosOffset.x *= -1.0f;
                };
            }
        }
    }
};

#endif //FIGHTER_IMPL