#include <openvdb/Types.h>
#include "Viewer.h"
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <typeinfo>
#ifdef DWA_OPENVDB
#include <logging_base/logging.h>
#include <usagetrack.h>
#endif
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/math/Transform.h>
#include <GLUT/GLUT.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <Mesh.h>
#include <openvdb/tools/VolumeToMesh.h>

extern MorphoGrid::Ptr mPtr;


void
usage(const char* progName, int status)
{
    (status == EXIT_SUCCESS ? std::cout : std::cerr) <<
    "Usage: " << progName << " file.vdb [file.vdb ...] [options]\n" <<
    "Which: displays OpenVDB grids\n" <<
    "Options:\n" <<
    "    -i            print grid info\n" <<
    "    -d            print debugging info\n" <<
    "Controls:\n" <<
    "    Esc                exit\n" <<
    "    -> (Right)         show next grid\n" <<
    "    <- (Left)          show previous grid\n" <<
    "    1                  toggle tree topology view on/off\n" <<
    "    2                  toggle surface view on/off\n" <<
    "    3                  toggle data view on/off\n" <<
    "    G                  (\"geometry\") look at center of geometry\n" <<
    "    H                  (\"home\") look at origin\n" <<
    "    I                  toggle on-screen grid info on/off\n" <<
    "    D                  Dilatation en 26 connexité\n" <<
    "    S                  Dilatation en 18 connexité\n" <<
    "    Q                  Dilatation en 6 connexité\n" <<
    "    E                  Erosion en 26 connexité\n" <<
    "    Z                  Erosion en 18 connexité\n" <<
    "    A                  Erosion en 6 connexité\n" <<
    "    left mouse         tumble\n" <<
    "    right mouse        pan\n" <<
    "    mouse wheel        zoom\n" <<
    "\n" <<
    "    X + wheel          move right cut plane\n" <<
    "    Shift + X + wheel  move left cut plane\n" <<
    "    Y + wheel          move top cut plane\n" <<
    "    Shift + Y + wheel  move bottom cut plane\n" <<
    "    Z + wheel          move front cut plane\n" <<
    "    Shift + Z + wheel  move back cut plane\n" <<
    "    Ctrl + X + wheel   move both X cut planes\n" <<
    "    Ctrl + Y + wheel   move both Y cut planes\n" <<
    "    Ctrl + Z + wheel   move both Z cut planes\n";
    exit(status);
}


////////////////////////////////////////

static Mesh mesh;

int
main(int argc, char *argv[])
{
    
#ifdef DWA_OPENVDB
    USAGETRACK_report_basic_tool_usage(argc, argv, /*duration=*/0);
    logging_base::configure(argc, argv);
#endif
    
    const char* progName = argv[0];
    if (const char* ptr = ::strrchr(progName, '/')) progName = ptr + 1;
    
    int status = EXIT_SUCCESS;
    
    try {
        openvdb::initialize();
        
        bool printInfo = false, printDebugInfo = false;
        
        // Parse the command line.
        std::vector<std::string> filenames;
        for (int n = 1; n < argc; ++n) {
            std::string str(argv[n]);
            if (str[0] != '-') {
                filenames.push_back(str);
            } else if (str == "-i") {
                printInfo = true;
            } else if (str == "-d") {
                printDebugInfo = true;
            } else if (str == "-h" || str == "--help") {
                usage(progName, EXIT_SUCCESS);
            } else {
                usage(progName, EXIT_FAILURE);
            }
        }
        
        openvdb_viewer::Viewer viewer = openvdb_viewer::init(progName, /*bg=*/false);
        
        if (printDebugInfo) {
            std::cout << viewer.getVersionString() << std::endl;
        }
        
        const size_t numFiles = filenames.size();
        if (numFiles == 0) usage(progName, EXIT_FAILURE);
        
        std::vector<MorphoGrid::Ptr> mallGrids;
        openvdb::GridCPtrVec allGrids;
        
        // Load mesh.
        std::string indent(numFiles == 1 ? "" : "    ");
        for (size_t n = 0; n < numFiles; ++n) {
            mesh.loadOFF(filenames[n], 1);
        }
        
        //récupération des points et triangles
        std::vector<openvdb::Vec3s> points = mesh.V;
        std::vector<openvdb::Vec3I> triangles = mesh.T;
        
        //récupération du MorphoTree
        openvdb::math::Transform::Ptr mTransform =
        openvdb::math::Transform::createLinearTransform(1);
        mPtr = openvdb::tools::meshToLevelSet<MorphoGrid>(mTransform.operator*(), points, triangles);
        
        //afin d'être capable d'afficher même de petits objets, j'ai fixé un peu arbitraire
        if (mPtr->tree().leafCount()<20){
            mesh.loadOFF(filenames[0], 100);
            points = mesh.V;
            triangles = mesh.T;
            mPtr = openvdb::tools::meshToLevelSet<MorphoGrid>(mTransform.operator*(), points, triangles);
        }
                
       
        MorphoTree::Ptr tmpTree = mPtr->treePtr();
        
        //nombre de feuilles actives de l'arbre
        std::cout << tmpTree->leafCount() << std::endl;
        
        viewer.open();
        viewer.view(allGrids);
        
        openvdb_viewer::exit();
        
    } catch (const char* s) {
        OPENVDB_LOG_ERROR(progName << ": " << s);
        status = EXIT_FAILURE;
    } catch (std::exception& e) {
        OPENVDB_LOG_ERROR(progName << ": " << e.what());
        status = EXIT_FAILURE;
    }
    
    return status;
}
