/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

#ifndef __vtkIGTLToMRMLTrajectory_h
#define __vtkIGTLToMRMLTrajectory_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlTrajectoryMessage.h>

// MRML includes
#include <vtkMRMLAnnotationFiducialNode.h>
#include <vtkMRMLAnnotationHierarchyNode.h>
#include <vtkMRMLAnnotationLineDisplayNode.h>
#include <vtkMRMLAnnotationPointDisplayNode.h>
#include <vtkMRMLAnnotationRulerNode.h>
#include <vtkMRMLHierarchyNode.h>
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLTrajectory : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLTrajectory *New();
  vtkTypeMacro(vtkIGTLToMRMLTrajectory,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual const char*  GetIGTLName() VTK_OVERRIDE { return "TRAJ"; };
  virtual std::vector<std::string>  GetAllMRMLNames() VTK_OVERRIDE
  {
    this->MRMLNames.clear();
    this->MRMLNames.push_back("AnnotationHierarchyNode");
    return this->MRMLNames;
  }
  virtual vtkIntArray* GetNodeEvents() VTK_OVERRIDE;
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name) VTK_OVERRIDE;

  virtual int          IGTLToMRML(vtkMRMLNode* node) VTK_OVERRIDE;
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg,  bool useProtocolV2) VTK_OVERRIDE;
virtual int          UnpackIGTLMessage(igtl::MessageBase::Pointer buffer);

 protected:
  vtkIGTLToMRMLTrajectory();
  ~vtkIGTLToMRMLTrajectory();

  void AddTrajectoryElement(igtl::TrajectoryElement::Pointer& tElemt, vtkMRMLAnnotationHierarchyNode* hierarchyNode);
  void UpdateRulerAnnotation(igtl::TrajectoryElement::Pointer& tElemt,vtkMRMLNode* node);
  void UpdateFiducialAnnotation(igtl::TrajectoryElement::Pointer& tElemt,vtkMRMLNode* node);

  int PrepareTrajectoryElement(igtl::TrajectoryElement::Pointer& tElemt, vtkMRMLNode* node);
  void CrossCheckTrajectoryName(igtl::TrajectoryElement::Pointer& tElemt,
				igtl::TrajectoryMessage::Pointer& trajMsg);

 protected:
  igtl::TrajectoryMessage::Pointer InTrajectoryMsg;
  igtl::TrajectoryMessage::Pointer OutTrajectoryMsg;

};


#endif //__vtkIGTLToMRMLTrajectory_h


