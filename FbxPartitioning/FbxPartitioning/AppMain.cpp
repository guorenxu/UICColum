
#define NOMINMAX

#include <iostream>

#include "MshReader.h"

#include "Types.h"
#include "Utilities.h"
#include "WorkflowScheduler.h"

#include "Workflows/FbxChunkExportWorkflow.h"
#include "Workflows/FbxImportWorkflow.h"
#include "Workflows/MshImportWorkflow.h"
#include "Workflows/PartitioningWorkflow.h"
#include "Workflows/TriangulationWorkflow.h"
#include "Workflows/RenderWorkflow.h"
#include "Workflows/DataGroupingWorkflow.h"
#include "Workflows/GatherMeshBoundsWorkflow.h"

#ifdef main
# undef main
#endif

//	Library references
#pragma comment( lib, "SDL2.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )



//	Toggles
#define PREVIEW_MESH



#pragma comment( lib, "libfbxsdk.lib" )

#pragma warning( disable: 4018 ) // signed/unsigned mismatch
#pragma warning( disable: 4244 ) // floating-point conversion warnings
#pragma warning( disable: 4267 ) // integer conversion warnings




//	MAIN PROCESSING FUNCTION

void process( std::string file )
{
	auto start = clock( );

	std::cout << "\n\n OPERATING ON " << file << std::endl;

	DataBinding<float_3> points;
	DataBinding<int> indices;
	DataBinding<triangle> mesh;
	DatumBinding<float_3> meshExtentsMin, meshExtentsMax;

	DataBinding<int> meshTags;

	DataBinding<mesh_chunk> partitionedMesh;

	auto ws = std::make_shared<Workspace>( );
	ws->AddData( points, indices, mesh, meshTags, partitionedMesh );

	WorkflowScheduler sched;
	sched.SetWorkspace( ws );

	sched.AddWorkflow( std::make_unique<Workflow>( new FbxImportWorkflow( "bsArteries.fbx", &points, &indices ) ) );
	sched.AddWorkflow( std::make_unique<Workflow>( new TriangulationWorkflow( points, indices, &mesh ) ) );
	//sched.AddWorkflow( std::make_unique<Workflow>( new RenderWorkflow( mesh ) ) );
	sched.AddWorkflow( std::make_unique<Workflow>( new GatherMeshBoundsWorkflow( mesh, &meshExtentsMin, &meshExtentsMax ) ) );
	sched.AddWorkflow( std::make_unique<Workflow>( new PartitioningWorkflow( mesh, meshExtentsMin, meshExtentsMax, &meshTags ) ) );
	sched.AddWorkflow( std::make_unique<Workflow>( new DataGroupingWorkflow( mesh, meshTags, &partitionedMesh ) ) );
	sched.AddWorkflow( std::make_unique<Workflow>( new FbxChunkExportWorkflow( "bsArteries", partitionedMesh ) ) );

	sched.Run( );

	std::cout << "\nDONE" << std::endl;
	std::cout << "Total operation took " << formatTime( clock( ) - start ) << std::endl;
}

//#ifdef _DEBUG
int main( )
//#else
//int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow )
//#endif
{
	auto start = clock( );

	//MshReader mshReader( "IanArteries5.GAMBIT.msh" );
	//return 0;

	//process( "bsArteries.fbx" );
	//process( "bsCSF.fbx" );
	process( "IanArteries5.GAMBIT.msh" );
	/*process( "bsGray.fbx" );
	process( "bsScalp.fbx" );
	process( "bsSkull.fbx" );
	process( "bsVeins.fbx" );
	process( "bsWhite.fbx" );*/

	auto end = clock( );

	std::cout << "\n\nTOTAL OPERATION TOOK: " << ( end - start ) / 1000 << "s" << std::endl;

	pause( );
	return 0;
}