/*
 *  LBfoam: An open-source software package for the simulation of foaming
 *  using the Lattice Boltzmann Method
 *  Copyright (C) 2020 Mohammadmehdi Ataei
 *  m.ataei@mail.utoronto.ca
 *  This file is part of LBfoam.
 *
 *  LBfoam is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU Affero General Public License as published by the
 *  Free Software Foundation version 3.
 *
 *  LBfoam is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this Program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  #############################################################################
 *
 *  Author:         Mohammadmehdi Ataei, 2020
 *
 *  #############################################################################
 *
 *  Parts of the LBfoam code that originate from Palabos are distributed
 *  under the terms of the AGPL 3.0 license with the following copyright
 *  notice:
 *
 *  This file is part of the Palabos library.
 *
 *  Copyright (C) 2011-2017 FlowKit Sarl
 *  Route d'Oron 2
 *  1010 Lausanne, Switzerland
 *  E-mail contact: contact@flowkit.com
 *
 *  The most recent release of Palabos can be downloaded at
 *  <http://www.palabos.org/>
 *
 *  The library Palabos is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  The library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BUBBLE_TRACKING_3D_H
#define BUBBLE_TRACKING_3D_H

#include "atomicBlock/atomicContainerBlock3D.h"
#include "atomicBlock/dataProcessingFunctional3D.h"
#include "atomicBlock/reductiveDataProcessingFunctional3D.h"
#include "core/globalDefs.h"
#include "multiBlock/reductiveMultiDataProcessorWrapper3D.h"
#include "multiPhysics/freeSurfaceModel3D.h"
#include "offLattice/makeSparse3D.h"
#include "parallelism/mpiManager.h"

namespace plb {
namespace lbfoam {

class BubbleMPIdata3D {
 public:
  BubbleMPIdata3D(MultiBlock3D &block);
  std::vector<plint> const &getLocalIds() const;

 private:
  void computeLocalIds(MultiBlock3D &block);

 private:
  std::vector<plint> localIds;
};

class BubbleTracking3D {
 public:
  BubbleTracking3D(MultiBlock3D &templ);
  ~BubbleTracking3D();
  template <typename T, template <typename U1> class FluidDescriptor,
            template <typename U2> class ScalarDescriptor>
  void execute(MultiScalarField3D<T> &volumeFraction,
               MultiScalarField3D<int> &flag, MultiTensorField3D<T, 3> &normal,
               MultiScalarField3D<T> &rhoBar, MultiScalarField3D<T> &mass,
               MultiTensorField3D<T, 3> &j,
               MultiBlockLattice3D<T, ScalarDescriptor> &adLattice,
               MultiScalarField3D<T> &oldVolumeFraction,
               bool incompressibleModel, T pi);

  MultiScalarField3D<plint> *getTagMatrix() { return tagMatrix; }
  void setTagMatrix(MultiScalarField3D<plint> *newTagMatrix) {
    tagMatrix = newTagMatrix;
  }
  std::vector<double> const &getBubbleVolume() { return bubbleVolume; }
  MultiScalarField3D<double> *setDisjoiningPressureFieldField() {
    return disjoiningPressureField;
  }
  void setDisjoiningPressureField(MultiScalarField3D<double> *newDisjoining) {
    disjoiningPressureField = newDisjoining;
  }
  std::vector<Array<double, 3>> const &getBubbleCenter() {
    return bubbleCenter;
  }
  pluint numBubbles() const { return bubbleVolume.size(); }
  std::vector<double> const &getBubbleDensity() { return bubbleDensity; }
  std::vector<double> const &getBubbleDisjoiningPressure() {
    return bubbleDisjoiningPressure;
  }

 private:
  // Re-assign a continuously numbered ID to the detected bubbles.
  pluint countAndTagBubbles();
  // Computes the volumes and centers of all new bubbles.
  template <typename T>
  void bubbleAnalysis(MultiScalarField3D<int> &flag,
                      MultiScalarField3D<T> &volumeFraction, pluint numBubbles);
  template <typename T, template <typename U1> class AD_Descriptor>
  void calculateBubbleGrowth(MultiScalarField3D<int> &flag,
                             MultiScalarField3D<T> &volumeFraction,
                             MultiScalarField3D<T> &oldVolumeFraction,
                             MultiBlockLattice3D<T, AD_Descriptor> &adLattice,
                             pluint numBubbles);
  template <typename T, template <typename U> class Descriptor>
  void stabilizeLattice(FreeSurfaceFields3D<T, Descriptor> &fields);
  // Implements all required MPI operations needed to compute the bubble volume
  // and centers, after calling AnalyzeBubbles3D.
  void computeBubbleData(pluint numBubbles);
  // Implements all required MPI operations needed to compute the global IDs of
  // the current bubbbles, after calling CollectBubbleTags3D.
  plint globalBubbleIds();
  // Prepare the bubble container for the next time iteration.
  void resetBubbleContainer();
  // Parallel bucket-fill algorithm to assign a unique ID to every contiguous
  // region.
  void bubbleBucketFill(MultiScalarField3D<int> &flag);

  // calculates DisjoiningPressure
  template <typename T, template <typename U> class Descriptor>
  void calculateDisjoiningPressure(T pi, MultiScalarField3D<int> &flag,
                                   MultiScalarField3D<T> &volumeFraction,
                                   MultiTensorField3D<T, 3> &normal,
                                   MultiScalarField3D<T> &rhoBar,
                                   MultiScalarField3D<T> &mass,
                                   MultiTensorField3D<T, 3> &j,
                                   bool incompressibleModel, pluint numBubbles);

 private:
  BubbleTracking3D(BubbleTracking3D const &rhs) : mpiData(rhs.mpiData) {
    PLB_ASSERT(false);
  }
  BubbleTracking3D &operator=(BubbleTracking3D const &rhs) {
    PLB_ASSERT(false);
    return *this;
  }

 private:
  MultiContainerBlock3D *bubbleContainer, *bubbleAnalysisContainer,
      *bubbleRemapContainer;
  BubbleMPIdata3D mpiData;
  MultiScalarField3D<plint> *tagMatrix;
  MultiScalarField3D<double> *disjoiningPressureField;
  std::vector<double> bubbleVolume;
  std::vector<double> bubbleDensity;
  std::vector<double> bubbleDisjoiningPressure;
  std::vector<Array<double, 3>> bubbleCenter;
  static const plint maxNumBubbles = 100000;
};

/**
 * Data for the bubble counter, associated to one block.
 * It holds information that changes during time:
 *  nextCellId: next available ID, if a new cell-type is found.
 *  retagging: a map that relates equivalent IDs, when two domains
 *             are found to be contiguous.
 *  maxNumBubbles: an upper bound for the number of bubbles per block,
 *                 so every block can create a globally unique bubble ID.
 **/
class BubbleCounterData3D : public ContainerBlockData {
 public:
  BubbleCounterData3D(plint maxNumBubbles_);
  virtual BubbleCounterData3D *clone() const;
  // The assumption here is that cell 0 is a cell that needs to
  // be tagged ("a bubble cell"). Depending on the tag of neighboring
  // cells, either one of the neighbor's tag or a new tag is assigned.
  // There is a conflict if cell 0 has been previously converted in
  // a way that is incompatible with the neighbor's tags.
  bool convertCell(plint &tag0, plint tag1, plint tag2, plint tag3, plint tag4,
                   plint tag5, plint tag6, plint tag7, plint tag8, plint tag9,
                   plint tag10, plint tag11, plint tag12, plint tag13,
                   plint tag1_, plint tag2_, plint tag3_, plint tag4_,
                   plint tag5_, plint tag6_, plint tag7_, plint tag8_,
                   plint tag9_, plint tag10_, plint tag11_, plint tag12_,
                   plint tag13_);
  // The assumption here is that cell 0 is a cell that needs to
  // be tagged ("a bubble cell"). Depending on the tag of neighboring
  // cell 1, either cell 1's tag or a new tag is assigned.
  // There is a conflict if cell 0 has been previously converted in
  // a way that is incompatible with cell 1's tag.
  bool processNeighbor(plint &tag0, plint tag1);
  plint getNextTag();
  void reset();
  // Get the "real" value (after remapping) of a given tag.
  plint convertTag(plint tag) const;
  // Important: it is assumed that oldTag and newTag are not
  // themselves mapped. This means that for arbitrary tags tag1 and
  // tag2 you should not call registerConflict(tag1,tag2), but
  // registerConflict(convertTag(tag1), convertTag(tag2).

  void registerConflict(plint oldTag, plint newTag);

 private:
  plint nextCellId;
  std::map<plint, plint> retagging;

 private:
  plint maxNumBubbles;
};

class BubbleRemapData3D : public ContainerBlockData {
 public:
  BubbleRemapData3D(plint maxNumBubbles_ = 0) : maxNumBubbles(maxNumBubbles_) {}
  virtual BubbleRemapData3D *clone() const;
  std::vector<plint> &getUniqueTags() { return uniqueTags; }
  std::vector<plint> const &getUniqueTags() const { return uniqueTags; }
  std::map<plint, plint> &getTagRemap() { return tagRemap; }
  bool isMyTag(plint tag);

 private:
  plint maxNumBubbles;
  std::vector<plint> uniqueTags;
  std::map<plint, plint> tagRemap;
};

struct BubbleAnalysisData3D : public ContainerBlockData {
  virtual BubbleAnalysisData3D *clone() const {
    return new BubbleAnalysisData3D(*this);
  }
  std::vector<double> bubbleVolume;
  std::vector<double> bubbleDensity;
  std::vector<double> bubbleDisjoiningPressure;
  std::vector<Array<double, 3>> bubbleCenter;
};

class CountBubbleIteration3D : public PlainReductiveBoxProcessingFunctional3D {
 public:
  CountBubbleIteration3D();
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> atomicBlocks);
  virtual CountBubbleIteration3D *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const {
    modified[0] = modif::staticVariables;  // tags.
    modified[1] = modif::nothing;          // flags.
    modified[2] = modif::nothing;          // data.
  }
  plint getNumConflicts() const;

 private:
  plint numConflictsId;
};

template <typename T>
class AnalyzeBubbles3D : public BoxProcessingFunctional3D {
 public:
  AnalyzeBubbles3D(pluint numBubbles_);
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> atomicBlocks);
  virtual AnalyzeBubbles3D<T> *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const {
    modified[0] = modif::staticVariables;  // tags.
    modified[1] = modif::nothing;          // flags.
    modified[2] = modif::nothing;          // data.
    modified[3] = modif::nothing;          // volume fraction.
  }

 private:
  pluint numBubbles;
};

// Converts the information about the overall available bubble tags available
// to all processors.
class CollectBubbleTags3D : public BoxProcessingFunctional3D {
 public:
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> atomicBlocks);
  virtual CollectBubbleTags3D *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const {
    modified[0] = modif::nothing;  // tags.
    modified[1] = modif::nothing;  // data.
  }
};

// Assign a new tag to all bubble cells (they must have been uniquely tagged
// previously). The only field in the BubbleCounterData3D which is used here is
// tagRemap.
class ApplyTagRemap3D : public BoxProcessingFunctional3D {
 public:
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> atomicBlocks);
  virtual ApplyTagRemap3D *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const {
    modified[0] = modif::staticVariables;  // tags.
    modified[1] = modif::nothing;          // data.
  }
};

template <typename T, template <typename U1> class AD_Descriptor>
class CalculateBubbleGrowth3D : public BoxProcessingFunctional3D {
 public:
  CalculateBubbleGrowth3D(pluint numBubbles_);
  //~CalculateBubbleGrowth3D();
  CalculateBubbleGrowth3D(CalculateBubbleGrowth3D<T, AD_Descriptor> const &rhs);
  CalculateBubbleGrowth3D<T, AD_Descriptor> &operator=(
      CalculateBubbleGrowth3D<T, AD_Descriptor> const &rhs);
  void swap(CalculateBubbleGrowth3D<T, AD_Descriptor> &rhs);
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> blocks);
  virtual CalculateBubbleGrowth3D<T, AD_Descriptor> *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const;

 private:
  pluint numBubbles;
};

template <typename T, template <typename U> class Descriptor>
class CalculateDisjoiningPressure3D : public BoxProcessingFunctional3D {
 public:
  CalculateDisjoiningPressure3D(T pi_, pluint numBubbles_,
                                bool incompressibleModel);
  CalculateDisjoiningPressure3D(
      CalculateDisjoiningPressure3D<T, Descriptor> const &rhs);
  CalculateDisjoiningPressure3D<T, Descriptor> &operator=(
      CalculateDisjoiningPressure3D<T, Descriptor> const &rhs);
  void swap(CalculateDisjoiningPressure3D<T, Descriptor> &rhs);
  virtual void processGenericBlocks(Box3D domain,
                                    std::vector<AtomicBlock3D *> blocks);
  virtual CalculateDisjoiningPressure3D<T, Descriptor> *clone() const;
  virtual void getTypeOfModification(
      std::vector<modif::ModifT> &modified) const;

 private:
  T pi;
  pluint numBubbles;
  bool incompressibleModel;
};

}  // namespace lbfoam
}  // namespace plb

#endif  // BUBBLE_TRACKING_3D_H
