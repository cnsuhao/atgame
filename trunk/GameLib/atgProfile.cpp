#include "atgBase.h"
#include "atgProfile.h"
#include "atgMisc.h"
#include "atgMath.h"

inline void Profile_Get_Ticks(uint64 * ticks)
{
    *ticks = GetAbsoluteMsecTime();
}

inline float Profile_Get_Tick_Rate(void)
{
//  return 1000000.f;
//  return 1000.f;
    return 1.0f;
}

/***************************************************************************************************
**
** CProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
CProfileNode::CProfileNode( const char * name, CProfileNode * parent ) :
    Name( name ),
    TotalCalls( 0 ),
    TotalTime( 0 ),
    StartTime( 0 ),
    RecursionCounter( 0 ),
    Parent( parent ),
    Child( NULL ),
    Sibling( NULL ),
    m_userPtr(0)
{
    Reset();
}


void CProfileNode::CleanupMemory()
{
    delete ( Child);
    Child = NULL;
    delete ( Sibling);
    Sibling = NULL;
}

CProfileNode::~CProfileNode( void )
{
    delete ( Child);
    delete ( Sibling);
}


/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - static string pointer to the name of the node we are searching for                   *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All profile names are assumed to be static strings so this function uses pointer compares   *
 * to find the named node.                                                                     *
 *=============================================================================================*/
CProfileNode * CProfileNode::Get_Sub_Node( const char * name )
{
    // Try to find this sub node
    CProfileNode * child = Child;
    while ( child ) {
        if ( child->Name == name ) {
            return child;
        }
        child = child->Sibling;
    }

    // We didn't find it, so add it
    
    CProfileNode * node = new CProfileNode( name, this );
    node->Sibling = Child;
    Child = node;
    return node;
}


void CProfileNode::Reset( void )
{
    TotalCalls = 0;
    TotalTime = 0.0f;
    

    if ( Child ) {
        Child->Reset();
    }
    if ( Sibling ) {
        Sibling->Reset();
    }
}


void CProfileNode::Call( void )
{
    TotalCalls++;
    if (RecursionCounter++ == 0) {
        Profile_Get_Ticks(&StartTime);
    }
}


bool CProfileNode::Return( void )
{
    if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
        uint64 time;
        Profile_Get_Ticks(&time);
        time-=StartTime;
        TotalTime += (float)time / Profile_Get_Tick_Rate();
    }
    return ( RecursionCounter == 0 );
}


/***************************************************************************************************
**
** CProfileIterator
**
***************************************************************************************************/
CProfileIterator::CProfileIterator( CProfileNode * start )
{
    CurrentParent = start;
    CurrentChild = CurrentParent->Get_Child();
}


void CProfileIterator::First(void)
{
    CurrentChild = CurrentParent->Get_Child();
}


void CProfileIterator::Next(void)
{
    CurrentChild = CurrentChild->Get_Sibling();
}


bool CProfileIterator::Is_Done(void)
{
    return CurrentChild == NULL;
}


void CProfileIterator::Enter_Child( int index )
{
    CurrentChild = CurrentParent->Get_Child();
    while ( (CurrentChild != NULL) && (index != 0) ) {
        index--;
        CurrentChild = CurrentChild->Get_Sibling();
    }

    if ( CurrentChild != NULL ) {
        CurrentParent = CurrentChild;
        CurrentChild = CurrentParent->Get_Child();
    }
}


void    CProfileIterator::Enter_Parent( void )
{
    if ( CurrentParent->Get_Parent() != NULL ) {
        CurrentParent = CurrentParent->Get_Parent();
    }
    CurrentChild = CurrentParent->Get_Child();
}


/***************************************************************************************************
 **
 ** CProfileManager
 **
 ***************************************************************************************************/

CProfileNode    CProfileManager::Root( "Root", NULL );
CProfileNode *  CProfileManager::CurrentNode = &CProfileManager::Root;
int             CProfileManager::FrameCounter = 0;
uint64          CProfileManager::ResetTime = 0;


/***********************************************************************************************
 * CProfileManager::Start_Profile -- Begin a named profile                                     *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a new child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
void    CProfileManager::Start_Profile( const char * name )
{
    if (name != CurrentNode->Get_Name()) {
        CurrentNode = CurrentNode->Get_Sub_Node( name );
    } 
    
    CurrentNode->Call();
}


/***********************************************************************************************
 * CProfileManager::Stop_Profile -- Stop timing and record the results.                        *
 *=============================================================================================*/
void    CProfileManager::Stop_Profile( void )
{
    // Return will indicate whether we should back up to our parent (we may
    // be profiling a recursive function)
    if (CurrentNode->Return()) {
        CurrentNode = CurrentNode->Get_Parent();
    }
}


/***********************************************************************************************
 * CProfileManager::Reset -- Reset the contents of the profiling system                        *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void CProfileManager::Reset( void )
{ 
    Root.Reset();
    Root.Call();
    FrameCounter = 0;
    Profile_Get_Ticks(&ResetTime);
}


/***********************************************************************************************
 * CProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void CProfileManager::Increment_Frame_Counter( void )
{
    FrameCounter++;
}


/***********************************************************************************************
 * CProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float CProfileManager::Get_Time_Since_Reset( void )
{
    uint64 time;
    Profile_Get_Ticks(&time);
    time -= ResetTime;
    return (float)time / Profile_Get_Tick_Rate();
}

#include <stdio.h>

void CProfileManager::dumpRecursive(CProfileIterator* profileIterator, int spacing)
{
    profileIterator->First();
    if (profileIterator->Is_Done())
        return;

    float accumulated_time=0,parent_time = profileIterator->Is_Root() ? CProfileManager::Get_Time_Since_Reset() : profileIterator->Get_Current_Parent_Total_Time();
    int i;
    int frames_since_reset = CProfileManager::Get_Frame_Count_Since_Reset();
    for (i=0;i<spacing;i++) LOG(".");
    LOG("----------------------------------\n");
    for (i=0;i<spacing;i++) LOG(".");
    LOG("Profiling: %s (total running time: %.3f ms) ---\n", profileIterator->Get_Current_Parent_Name(), parent_time );
    float totalTime = 0.f;

    
    int numChildren = 0;
    
    for (i = 0; !profileIterator->Is_Done(); i++,profileIterator->Next())
    {
        numChildren++;
        float current_total_time = profileIterator->Get_Current_Total_Time();
        accumulated_time += current_total_time;
        float fraction = parent_time > atgMath::EPSILON ? (current_total_time / parent_time) * 100.f : 0.f;
        {
            int i;  for (i=0;i<spacing;i++) LOG(".");
        }
        LOG("%d -- %s (%.2f %%) :: %.3f ms / frame (%d calls)\n",i, profileIterator->Get_Current_Name(), fraction,(current_total_time / (double)frames_since_reset),profileIterator->Get_Current_Total_Calls());
        totalTime += current_total_time;
        //recurse into children
    }

    if (parent_time < accumulated_time)
    {
        LOG("what's wrong\n");
    }
    for (i=0;i<spacing;i++) LOG(".");
    LOG("%s (%.3f %%) :: %.3f ms\n", "Unaccounted:",parent_time > atgMath::EPSILON ? ((parent_time - accumulated_time) / parent_time) * 100.f : 0.f, parent_time - accumulated_time);
    
    for (i=0;i<numChildren;i++)
    {
        profileIterator->Enter_Child(i);
        dumpRecursive(profileIterator,spacing+3);
        profileIterator->Enter_Parent();
    }
}

void CProfileManager::dumpAll()
{
    CProfileIterator* profileIterator = 0;
    profileIterator = CProfileManager::Get_Iterator();

    dumpRecursive(profileIterator,0);

    CProfileManager::Release_Iterator(profileIterator);
}


