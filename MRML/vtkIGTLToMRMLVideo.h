/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 Program:   3D Slicer
 Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLVideo.h $
 Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
 Version:   $Revision: 15552 $
 
 ==========================================================================*/

#ifndef __vtkIGTLToMRMLVideo_h
#define __vtkIGTLToMRMLVideo_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlVideoMessage.h>
#if OpenIGTLink_BUILD_H264
#include "H264Decoder.h"
#endif
#if OpenIGTLink_BUILD_VPX
#include "VPXDecoder.h"
#endif
#if OpenIGTLink_LINK_X265
#include "H265Decoder.h"
#endif


// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLVolumeNode.h>

// VTK includes
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include "vtkRenderer.h"
#include <vtkSmartPointer.h>
#include "vtkImageData.h"
#include "vtkInformation.h"
#include <vtkRenderWindow.h>

//VideoStreaming codec includes
#include "VideoStreamIGTLinkReceiver.h"

#define VideoThreadMaxNumber 5

class VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkIGTLToMRMLVideo : public vtkIGTLToMRMLBase
{
public:
  
  static vtkIGTLToMRMLVideo *New();
  vtkTypeMacro(vtkIGTLToMRMLVideo,vtkObject);
  
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual const char*  GetIGTLName() { return "VIDEO"; };
  virtual const char*  GetMRMLName() { return "VectorVolume"; };
  virtual vtkIntArray* GetNodeEvents();
  virtual vtkMRMLNode* CreateNewNode(vtkMRMLScene* scene, const char* name);
  virtual int          IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node, int modify);
  virtual int          MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg);
  vtkMRMLNode* CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingVideoMessage);
  void SetDefaultDisplayNode(vtkMRMLVolumeNode *volumeNode, int numberOfComponents);
  
  void CenterImage(vtkMRMLVolumeNode *volumeNode);
  
  GenericDecoder* VideoStreamDecoderVPX[VideoThreadMaxNumber];
  
  GenericDecoder* VideoStreamDecoderX265[VideoThreadMaxNumber];
  
  GenericDecoder* VideoStreamDecoderH264[VideoThreadMaxNumber];
  
  SourcePicture* pDecodedPic;
  
protected:
  vtkIGTLToMRMLVideo();
  ~vtkIGTLToMRMLVideo();
  
  int IGTLToVTKScalarType(int igtlType);
  
protected:
  igtl::VideoMessage::Pointer OutVideoMsg;
  
  igtl::StartVideoDataMessage::Pointer StartVideoMsg;
  igtl::StopVideoMessage::Pointer StopVideoMsg;
  igtl::VideoMessage::Pointer videoMsg;
  
};


#endif //__vtkIGTLToMRMLVideo_h