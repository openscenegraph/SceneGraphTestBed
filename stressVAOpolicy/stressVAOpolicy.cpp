
/// This program test robustness of VAS path
/// it collects geometries given in arg then add and remove them at runtime
/// stressing osg vas policy as well as driver vao policy

#include <osgUtil/MeshOptimizers>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>

class GeomLoaderCB : public  osg::NodeCallback
{
    int _thresremoval; int _nbaddedatatime;
    std::list<osg::ref_ptr<osg::Geometry> > _geoms;
public:
    GeomLoaderCB(int thresremoval=1, int nbaddedatatime=1):_nbaddedatatime(nbaddedatatime), _thresremoval(thresremoval) {}

    void setGeometryList(const osgUtil::GeometryCollector::GeometryList& geomlist) {
        for(auto geom : geomlist)  _geoms.push_back(geom);
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
        osg::Group*  gr = node->asGroup();
        if(!gr || _geoms.empty() ) return;

        if(gr->getNumChildren()>_thresremoval)
        {
            osg::Geometry* removedgeom = gr->getChild(0)->asGeometry();
            if(removedgeom) {
                OSG_WARN<<"removing "<< removedgeom <<std::endl;
                gr->removeChildren(0,1);
            }
            return;
        }

        std::list<osg::ref_ptr<osg::Geometry> > ::iterator it= _geoms.begin();
        int cpt=0;
        while(it!=_geoms.end() && cpt++<_nbaddedatatime ) {
            osg::Geometry *addedgeom = static_cast<osg::Geometry*>((*it)->clone(osg::CopyOp::DEEP_COPY_ALL));
            addedgeom->setUseDisplayList(false);
            addedgeom->setUseVertexBufferObjects(true);
            addedgeom->setUseVertexArrayObject(true);
            gr->addChild(addedgeom);
            OSG_WARN<<"add "<< addedgeom <<std::endl;
            it=_geoms.erase(it);
        }
        return;
    }

};


int main(int argc, char **argv)
{
    osg::ArgumentParser args(&argc,argv);
    unsigned int  geomcountaddedatatime=1, geomcountabovewichweremove=2;
    while(args.read("--add",geomcountaddedatatime) ) { }
    while(args.read("--remove",geomcountabovewichweremove) ) { }
    osgUtil::GeometryCollector geomcollector(0,osgUtil::Optimizer::ALL_OPTIMIZATIONS);

    osg::ref_ptr<osg::Node > loaded = osgDB::readNodeFiles(args);
    if(!loaded.valid())  loaded = osgDB::readNodeFile("testdata/BIGCITY.ive");
    if(loaded.valid())
    {
        loaded->accept(geomcollector);

        osg::Group * root=new osg::Group;
        root->setDataVariance(osg::Object::DYNAMIC);
        GeomLoaderCB * loader=new GeomLoaderCB(geomcountabovewichweremove,geomcountaddedatatime);
        loader->setGeometryList( geomcollector.getGeometryList() );
        loaded=0;
        root->setUpdateCallback(loader);

        osgViewer::Viewer viewer;
        viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
        viewer.addEventHandler(new osgViewer::StatsHandler);
        viewer.addEventHandler(new osgViewer::WindowSizeHandler);
        viewer.addEventHandler(new osgViewer::ThreadingHandler);
        viewer.setRunFrameScheme(osgViewer::ViewerBase::CONTINUOUS);

        viewer.setSceneData(root);

        viewer.realize();
        viewer.run();
    }

}
