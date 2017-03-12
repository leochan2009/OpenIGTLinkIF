/*==========================================================================

  Portions (c) Copyright 2008-2014 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    vtkIGTLToMRMLPolyData.h

==========================================================================*/

#ifndef __vtkIGTLToMRMLPolyData_h
#define __vtkIGTLToMRMLPolyData_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlPolyDataMessage.h>

// OpenIGTLinkIO include
#include "igtlioPolyDataConverter.h"

// MRML includes
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObject.h>

class vtkCellArray;
class vtkDataSetAttributes;

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLPolyData : public vtkIGTLToMRMLBase
{
 public:

  static vtkIGTLToMRMLPolyData *New();
  vtkTypeMacro(vtkIGTLToMRMLPolyData,vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char*  GetIGTLName() { return "POLYDATA"; };
  virtual const char*  GetMRMLName() { return "Model"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name,
                                                igtl::MessageBase::Pointer incomingPolyDataMessage);

  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);


 protected:
  vtkIGTLToMRMLPolyData();
  ~vtkIGTLToMRMLPolyData();

  igtl::PolyDataMessage::Pointer OutPolyDataMessage;
  igtl::GetPolyDataMessage::Pointer GetPolyDataMessage;
  
  igtlio::PolyDataConverter*  converter;
  
  igtlio::PolyDataConverter::MessageContent* vtkContent;

};


#endif //__vtkIGTLToMRMLPolyData_h
