/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

==========================================================================*/

#ifndef __vtkIGTLToMRMLPosition_h
#define __vtkIGTLToMRMLPosition_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlPositionMessage.h>

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLPosition : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLPosition *New();
  vtkTypeMacro(vtkIGTLToMRMLPosition,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POSITION"; };
  virtual const char*  GetMRMLName() { return "LinearTransform"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node, int modify);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLPosition();
  ~vtkIGTLToMRMLPosition();

 protected:
  igtl::PositionMessage::Pointer OutPositionMsg;

};


#endif //__vtkIGTLToMRMLPosition_h
