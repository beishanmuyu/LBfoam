namespace plb {
    template<typename T> class MultiNTensorField3D;
}
/*
 * blockLattice3d/isoThermalDynamics.i
 */

%{
#include "PALABOS_ROOT/src/latticeBoltzmann/nearestNeighborLattices3D.h"
#include "PALABOS_ROOT/src/latticeBoltzmann/nearestNeighborLattices3D.hh"
#include "PALABOS_ROOT/src/basicDynamics/isoThermalDynamics.h"
#include "JLABOS_ROOT/plbWrapper/lattice/dynamicsGenerator.h"
%}


namespace plb {

template<typename T, class Descriptor>
struct BGKdynamics : public Dynamics<T,Descriptor> {
    BGKdynamics(T omega);
    virtual int getId() const;
};

template<typename T, class Descriptor>
struct ExternalMomentBGKdynamics : public Dynamics<T,Descriptor> {
    ExternalMomentBGKdynamics(T omega);
    virtual int getId() const;
};

template<typename T, class Descriptor>
struct ConstRhoBGKdynamics : public Dynamics<T,Descriptor> {
    ConstRhoBGKdynamics(T omega);
    virtual int getId() const;
};

template<typename T, class Descriptor>
struct RLBdynamics : public Dynamics<T,Descriptor> {
    virtual int getId() const;
private:
    RLBdynamics();
};

%newobject generateRLBdynamics;
template<typename T, class Descriptor>
RLBdynamics<T,Descriptor>* generateRLBdynamics(Dynamics<T,Descriptor>* baseDynamics);

template<typename T, class Descriptor>
struct RegularizedBGKdynamics : public Dynamics<T,Descriptor> {
    RegularizedBGKdynamics(T omega);
    virtual int getId() const;
};

template<typename T, class Descriptor>
struct ExternalMomentRegularizedBGKdynamics : public Dynamics<T,Descriptor> {
    ExternalMomentRegularizedBGKdynamics(T omega);
    virtual int getId() const;
};

template<typename T, class Descriptor>
struct ChopardDynamics : public Dynamics<T,Descriptor> {
    ChopardDynamics(T vs2, T omega);
    virtual int getId() const;
};

}  // namespace plb

%template(FLOAT_T_DESCRIPTOR_3D_PlbBGKdynamics) plb::BGKdynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;
//%template(PlbExternalMomentBGKdynamics) plb::ExternalMomentBGKdynamics<FLOAT_T,plb::descriptors::EXTMOMENT_DESCRIPTOR_3D>;
%template(FLOAT_T_DESCRIPTOR_3D_PlbConstRhoBGKdynamics) plb::ConstRhoBGKdynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;
%template(FLOAT_T_DESCRIPTOR_3D_PlbRLBdynamicsImpl) plb::RLBdynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;
%template(FLOAT_T_DESCRIPTOR_3D_PlbRLBdynamics) plb::generateRLBdynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;
%template(FLOAT_T_DESCRIPTOR_3D_PlbRegularizedBGKdynamics) plb::RegularizedBGKdynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;
//%template(PlbExternalMomentRegularizedBGKdynamics3D) plb::ExternalMomentRegularizedBGKdynamics<FLOAT_T,plb::descriptors::EXTMOMENT_DESCRIPTOR_3D>;
%template(FLOAT_T_DESCRIPTOR_3D_PlbChopardDynamics) plb::ChopardDynamics<FLOAT_T,plb::descriptors::DESCRIPTOR_3D>;

