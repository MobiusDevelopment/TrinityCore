/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef _OBJECT_POS_SELECTOR_H
#define _OBJECT_POS_SELECTOR_H

#include "Common.h"
#include <map>
#include <cmath>

enum UsedPosType { USED_POS_PLUS, USED_POS_MINUS };

inline UsedPosType operator ~(UsedPosType uptype)
{
    return uptype==USED_POS_PLUS ? USED_POS_MINUS : USED_POS_PLUS;
}

struct GAME_API ObjectPosSelector
{
    struct UsedPos
    {
        UsedPos(float sign_, float size_, float dist_) : sign(sign_), size(size_), dist(dist_) { }

        float sign;

        float size;                                         // size of point
        float dist;                                         // dist to central point (including central point size)
    };

    typedef std::multimap<float, UsedPos> UsedPosList;       // abs(angle)->Node

    ObjectPosSelector(float x, float y, float size, float dist);

    void AddUsedPos(float size, float angle, float dist);
    void InitializeAngle();

    bool FirstAngle(float& angle);
    bool NextAngle(float& angle);
    bool NextUsedAngle(float& angle);

    bool NextPosibleAngle(float& angle);

    bool CheckAngle(UsedPosList::value_type const& nextUsedPos, float sign, float angle ) const
    {
        float angle_step2  = GetAngle(nextUsedPos.second);

        float next_angle = nextUsedPos.first;
        if (nextUsedPos.second.sign * sign < 0)                       // last node from diff. list (-pi+alpha)
            next_angle = 2 * float(M_PI) - next_angle;   // move to positive

        return std::fabs(angle) + angle_step2 <= next_angle;
    }

    bool CheckOriginal() const
    {
        return (m_UsedPosLists[USED_POS_PLUS].empty() || CheckAngle(*m_UsedPosLists[USED_POS_PLUS].begin(), 1.0f, 0)) &&
            (m_UsedPosLists[USED_POS_MINUS].empty() || CheckAngle(*m_UsedPosLists[USED_POS_MINUS].begin(), -1.0f, 0));
    }

    bool IsNonBalanced() const { return m_UsedPosLists[USED_POS_PLUS].empty() != m_UsedPosLists[USED_POS_MINUS].empty(); }

    bool NextAngleFor(UsedPosList::value_type const& usedPos, float sign, UsedPosType uptype, float &angle)
    {
        float angle_step  = GetAngle(usedPos.second);

        // next possible angle
        angle  = usedPos.first * usedPos.second.sign + angle_step * sign;

        UsedPosList::value_type const* nextNode = nextUsedPos(uptype);
        if (nextNode)
        {
            // if next node permit use selected angle, then do it
            if (!CheckAngle(*nextNode, sign, angle))
            {
                m_smallStepOk[uptype] = false;
                return false;
            }
        }

        // possible more points
        m_smallStepOk[uptype] = true;
        m_smallStepAngle[uptype] = angle;
        m_smallStepNextUsedPos[uptype] = nextNode;

        return true;
    }

    bool NextSmallStepAngle(float sign, UsedPosType uptype, float &angle)
    {
        // next possible angle
        angle  = m_smallStepAngle[uptype] + m_anglestep * sign;

        if (std::fabs(angle) > float(M_PI))
        {
            m_smallStepOk[uptype] = false;
            return false;
        }

        if (m_smallStepNextUsedPos[uptype])
        {
            if (std::fabs(angle) >= m_smallStepNextUsedPos[uptype]->first)
            {
                m_smallStepOk[uptype] = false;
                return false;
            }

            // if next node permit use selected angle, then do it
            if (!CheckAngle(*m_smallStepNextUsedPos[uptype], sign, angle))
            {
                m_smallStepOk[uptype] = false;
                return false;
            }
        }

        // possible more points
        m_smallStepAngle[uptype] = angle;
        return true;
    }

    // next used post for m_nextUsedPos[uptype]
    UsedPosList::value_type const* nextUsedPos(UsedPosType uptype);

    // angle from used pos to next possible free pos
    float GetAngle(UsedPos const& usedPos) const { return std::acos(m_dist/(usedPos.dist+usedPos.size+m_size)); }

    float m_center_x;
    float m_center_y;
    float m_size;                                           // size of object in center
    float m_dist;                                           // distance for searching pos (including central object size)
    float m_anglestep;

    UsedPosList m_UsedPosLists[2];
    UsedPosList::const_iterator m_nextUsedPos[2];

    // field for small step from first after next used pos until next pos
    float m_smallStepAngle[2];
    bool  m_smallStepOk[2];
    UsedPosList::value_type const* m_smallStepNextUsedPos[2];
};
#endif
