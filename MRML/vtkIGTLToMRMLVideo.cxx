/*==========================================================================
 
 Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 Program:   3D Slicer
 Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLVideo.cxx $
 Date:      $Date: 2010-12-07 21:39:19 -0500 (Tue, 07 Dec 2010) $
 Version:   $Revision: 15621 $
 
 ==========================================================================*/

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLVideo.h"
#include "vtkMRMLIGTLQueryNode.h"

// OpenIGTLink includes
#include <igtl_util.h>
#include <igtlVideoMessage.h>

// Slicer includes
//#include <vtkSlicerColorLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkMRMLColorTableNode.h>

// MRML includes
#include <vtkMRMLVectorVolumeNode.h>
#include <vtkMRMLVectorVolumeDisplayNode.h>
#include "vtkMRMLBitStreamNode.h"

// VTK includes
#include <vtkMatrix4x4.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

#include "vtkSlicerOpenIGTLinkIFLogic.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLToMRMLVideo);
//---------------------------------------------------------------------------
vtkIGTLToMRMLVideo::vtkIGTLToMRMLVideo()
{
  for (int i = 0; i< VideoThreadMaxNumber; i++)
  {
    char *configFile[]={(char *)"",(char *)""};
    VideoStreamDecoder[i] = new VideoStreamIGTLinkReceiver(configFile);
  }
  
}

//---------------------------------------------------------------------------
vtkIGTLToMRMLVideo::~vtkIGTLToMRMLVideo()
{
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLVideo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

vtkMRMLNode* vtkIGTLToMRMLVideo::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkMRMLAnnotationHierarchyNode* hierarchyNode;
  
  hierarchyNode = vtkMRMLAnnotationHierarchyNode::New();
  hierarchyNode->SetName(name);
  hierarchyNode->SetDescription("Received by OpenIGTLink");
  hierarchyNode->SetParentNodeID(NULL);
  hierarchyNode->HideFromEditorsOff();
  
  vtkMRMLNode* n = scene->AddNode(hierarchyNode);
  hierarchyNode->Delete();
  
  return n;
}

//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLVideo::GetNodeEvents()
{
  vtkIntArray* events;
  
  events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLHierarchyNode::ChildNodeAddedEvent);
  events->InsertNextValue(vtkMRMLHierarchyNode::ChildNodeRemovedEvent);
  
  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLVideo::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  // Create a message buffer to receive image data
  vtkMRMLVectorVolumeNode* volumeNode = vtkMRMLVectorVolumeNode::SafeDownCast(node);
  std::string nodeName(volumeNode->GetName());
  nodeName.append("_BitStream");
  //vtkCollection* collection =  volumeNode->GetScene()->GetNodesByClassByName("vtkMRMLBitStreamNode", nodeName.c_str());
  const char* bitStreamNodeID = volumeNode->GetAttribute("vtkMRMLVectorVolumeNode.rel_bitStreamID");
  if(bitStreamNodeID)
  {
    vtkMRMLBitStreamNode* bitStreamNode = vtkMRMLBitStreamNode::SafeDownCast(volumeNode->GetScene()->GetNodeByID(bitStreamNodeID));
    igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
    videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    igtl::ImageMessage::Pointer imageMsg = igtl::ImageMessage::New();
    if(strcmp(buffer->GetDeviceType(),"VIDEO")==0)
    {
      videoMsg->SetMessageHeader(buffer);
      videoMsg->AllocateBuffer(); // fix it, copy buffer doesn't work
      memcpy(videoMsg->GetPackBodyPointer(),(unsigned char*) buffer->GetPackBodyPointer(),buffer->GetBodySizeToRead());// !! TODO: copy makes performance issue.
      // TODO, for multiple video transmission, use meta infomation to link message to different volume node.
      // generate multiple volumenodes to connect to different video sources
      
      
      // Deserialize the transform data
      // If CheckCRC==0, CRC check is skipped.
      int c = videoMsg->Unpack();
      
      if (c == 0) // if CRC check fails
      {
        // TODO: error handling
        return 0;
      }
      std::string deviceName(buffer->GetDeviceName()); // buffer has the header information, the videoMsg has not device name information.
      int currentDecoderIndex = -1;
      for (int i = 0; i < VideoThreadMaxNumber; i++)
      {
        if (deviceName.compare(VideoStreamDecoder[i]->deviceName) == 0)
        {
          currentDecoderIndex = i;
          break;
        }
      }
      if (currentDecoderIndex<0)
      {
        for (int i = 0; i < VideoThreadMaxNumber; i++)
        {
          if (VideoStreamDecoder[i]->deviceName.compare("") == 0)
          {
            currentDecoderIndex = i;
            break;
          }
        }
        VideoStreamDecoder[currentDecoderIndex]->deviceName = deviceName;
      }
      if(currentDecoderIndex>=0)
      {
        vtkSmartPointer<vtkImageData> imageData = volumeNode->GetImageData();
        int32_t Width = videoMsg->GetWidth();
        int32_t Height = videoMsg->GetHeight();
        if (videoMsg->GetWidth() != imageData->GetDimensions()[0] ||
            videoMsg->GetHeight() != imageData->GetDimensions()[1])
        {
          imageData->SetDimensions(Width , Height, 1);
          imageData->SetExtent(0, Width-1, 0, Height-1, 0, 0 );
          imageData->SetOrigin(0, 0, 0);
          imageData->AllocateScalars(VTK_UNSIGNED_CHAR,3);
        }
        VideoStreamDecoder[currentDecoderIndex]->SetWidth(Width);
        VideoStreamDecoder[currentDecoderIndex]->SetHeight(Height);
        int streamLength = videoMsg->GetBitStreamSize();
        VideoStreamDecoder[currentDecoderIndex]->SetStreamLength(streamLength);
        
        char* bitstream = new char[streamLength];
        memcpy(bitstream, videoMsg->GetPackFragmentPointer(2), streamLength);
        bitStreamNode->SetMessageStream(buffer);
        if(!VideoStreamDecoder[currentDecoderIndex]->ProcessVideoStream((igtl_uint8*)bitstream))
        {
          delete[] bitstream;
          return 0;
        }
        igtl_uint16 frameType = videoMsg->GetFrameType();
        
        if(frameType > 0x00FF)
        {
          VideoStreamDecoder[currentDecoderIndex]->IsGrayImage =  true;
          frameType = frameType >> 8;
        }
        else
        {
          VideoStreamDecoder[currentDecoderIndex]->IsGrayImage =  false;
        }
        if(frameType==videoFrameTypeIDR)
        {
          bitStreamNode->SetKeyFrameDecodedFlag(true);
        }
        else
        {
          bitStreamNode->SetKeyFrameDecodedFlag(false);
        }
        delete[] bitstream;
        if (VideoStreamDecoder[currentDecoderIndex]->IsGrayImage)
        {
          VideoStreamDecoder[currentDecoderIndex]->YUV420ToGrayImageConversion((uint8_t*)imageData->GetScalarPointer(), VideoStreamDecoder[currentDecoderIndex]->decodedNal, Height, Width);
        }
        else
        {
          VideoStreamDecoder[currentDecoderIndex]->YUV420ToRGBConversion((uint8_t*)imageData->GetScalarPointer(), VideoStreamDecoder[currentDecoderIndex]->decodedNal, Height, Width);
        }
        imageData->Modified();
        volumeNode->SetAndObserveImageData(imageData);
        volumeNode->Modified();
        //volumeNode->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent);
        return 1;
      }
    }
    else if(strcmp(buffer->GetDeviceType(),"IMAGE")==0)
    {
      imageMsg->SetMessageHeader(buffer);
      imageMsg->AllocateBuffer(); // fix it, copy buffer doesn't work
      memcpy(imageMsg->GetPackBodyPointer(),(unsigned char*) buffer->GetPackBodyPointer(),buffer->GetBodySizeToRead());// !! TODO: copy makes performance issue.
      // TODO, for multiple video transmission, use meta infomation to link message to different volume node.
      // generate multiple volumenodes to connect to different video sources
      
      
      // Deserialize the transform data
      // If CheckCRC==0, CRC check is skipped.
      int c = imageMsg->Unpack();
      
      if (c == 0) // if CRC check fails
      {
        // TODO: error handling
        return 0;
      }
      int size[3];
      vtkSmartPointer<vtkImageData> imageData = volumeNode->GetImageData();
      imageMsg->GetDimensions(size);
      int32_t Width = size[0];
      int32_t Height = size[1];
      if (Width!= imageData->GetDimensions()[0] ||
          Height != imageData->GetDimensions()[1])
      {
        imageData->SetDimensions(Width , Height, 1);
        imageData->SetExtent(0, Width-1, 0, Height-1, 0, 0 );
        imageData->SetOrigin(0, 0, 0);
        imageData->AllocateScalars(VTK_UNSIGNED_CHAR,3);
      }
      memcpy(imageData->GetScalarPointer(),
             imageMsg->GetScalarPointer(), imageMsg->GetSubVolumeImageSize());
      imageData->Modified();
      volumeNode->SetAndObserveImageData(imageData);
      volumeNode->Modified();
    }
  }
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLVideo::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
  {
    return 0;
  }
  if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0)   // If mrmlNode is query node
  {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
    {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
      {
        if (this->StartVideoMsg.IsNull())
        {
          this->StartVideoMsg = igtl::StartVideoDataMessage::New();
        }
        this->StartVideoMsg->SetDeviceName(qnode->GetIGTLDeviceName());
        this->StartVideoMsg->Pack();
        *size = this->StartVideoMsg->GetPackSize();
        *igtlMsg = this->StartVideoMsg->GetPackPointer();
      }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
      {
        if (this->StopVideoMsg.IsNull())
        {
          this->StopVideoMsg = igtl::StopVideoMessage::New();
        }
        this->StopVideoMsg->SetDeviceName(qnode->GetIGTLDeviceName());
        this->StopVideoMsg->Pack();
        *size = this->StopVideoMsg->GetPackSize();
        *igtlMsg = this->StopVideoMsg->GetPackPointer();
      }
      return 1;
    }
  }
  return 0;
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLVideo::CreateNewNodeWithMessage(vtkMRMLScene* scene, const char* name, igtl::MessageBase::Pointer incomingVideoMessage)
{
  vtkMRMLVectorVolumeNode* volumeNode = vtkMRMLVectorVolumeNode::New();
  scene->SaveStateForUndo();
  volumeNode = vtkMRMLVectorVolumeNode::New();
  volumeNode->SetName(name);
  vtkMRMLBitStreamNode* bitStreamNode = vtkMRMLBitStreamNode::New();
  std::string nodeName(name);
  nodeName.append("_BitStream");
  bitStreamNode->SetName(nodeName.c_str());
  bitStreamNode->SetVectorVolumeNode(volumeNode);
  scene->AddNode(bitStreamNode);
  volumeNode->SetAttribute("vtkMRMLVectorVolumeNode.rel_bitStreamID", bitStreamNode->GetID());
  bitStreamNode->SetVideoMessageConverter(this);
  volumeNode->AddObserver(vtkMRMLVolumeNode::ImageDataModifiedEvent, bitStreamNode, &vtkMRMLBitStreamNode::ProcessMRMLEvents);
  
  int i = 0;
  for (i = 0; i< VideoThreadMaxNumber; i++)
  {
    if (VideoStreamDecoder[i]->deviceName.compare("")==0)
    {
      VideoStreamDecoder[i]->deviceName = name;
      break;
    }
  }
  int32_t Width = 0;
  int32_t Height = 0;
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
  videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  igtl::ImageMessage::Pointer imageMsg = igtl::ImageMessage::New();
  if(strcmp(incomingVideoMessage->GetDeviceType(),"Video")==0)
  {
    videoMsg->SetMessageHeader(incomingVideoMessage);
    videoMsg->AllocateBuffer(); // fix it, copy buffer doesn't work
    memcpy(videoMsg->GetPackBodyPointer(),(unsigned char*) incomingVideoMessage->GetPackBodyPointer(),incomingVideoMessage->GetBodySizeToRead());// !! TODO: copy makes
    int c = videoMsg->Unpack();
    if(c>0)
    {
      Width = videoMsg->GetWidth();
      Height = videoMsg->GetHeight();
    }
  }
  else if(strcmp(incomingVideoMessage->GetDeviceType(),"IMAGE")==0)
  {
    imageMsg->SetMessageHeader(incomingVideoMessage);
    imageMsg->AllocateBuffer(); // fix it, copy buffer doesn't work
    memcpy(imageMsg->GetPackBodyPointer(),(unsigned char*) incomingVideoMessage->GetPackBodyPointer(),incomingVideoMessage->GetBodySizeToRead());// !! TODO: copy makes
    int c = imageMsg->Unpack();
    if(c>0)
    {
      int size[3];
      imageMsg->GetDimensions(size);
      Width = size[0];
      Height = size[1];
    }
  }
  image->SetDimensions(Width , Height, 1);
  image->SetExtent(0, Width-1, 0, Height-1, 0, 0 );
  image->SetOrigin(0, 0, 0);
  image->AllocateScalars(VTK_UNSIGNED_CHAR,3);
  
  volumeNode->SetAndObserveImageData(image);
  
  scene->SaveStateForUndo();
  vtkDebugMacro("Setting scene info");
  volumeNode->SetScene(scene);
  volumeNode->SetDescription("Received by OpenIGTLink");
  vtkMRMLNode* n = scene->AddNode(volumeNode);
  this->SetDefaultDisplayNode(volumeNode,3);
  this->CenterImage(volumeNode);
  return n;
}

//---------------------------------------------------------------------------
void vtkIGTLToMRMLVideo::CenterImage(vtkMRMLVolumeNode *volumeNode)
{
  if ( volumeNode )
  {
    vtkImageData *image = volumeNode->GetImageData();
    if (image)
    {
      vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
      volumeNode->GetIJKToRASMatrix(ijkToRAS);
      
      double dimsH[4];
      double rasCorner[4];
      int *dims = image->GetDimensions();
      dimsH[0] = dims[0] - 1;
      dimsH[1] = dims[1] - 1;
      dimsH[2] = dims[2] - 1;
      dimsH[3] = 0.;
      ijkToRAS->MultiplyPoint(dimsH, rasCorner);
      
      double origin[3];
      int i;
      for (i = 0; i < 3; i++)
      {
        origin[i] = -0.5 * rasCorner[i];
      }
      volumeNode->SetDisableModifiedEvent(1);
      volumeNode->SetOrigin(origin);
      volumeNode->SetDisableModifiedEvent(0);
      volumeNode->InvokePendingModifiedEvent();
      
      ijkToRAS->Delete();
    }
  }
}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLVideo::SetDefaultDisplayNode(vtkMRMLVolumeNode *volumeNode, int numberOfComponents)
{
  if (volumeNode==NULL)
  {
    vtkWarningMacro("Failed to create display node for volume node");
    return;
  }
  
  vtkMRMLScene* scene=volumeNode->GetScene();
  if (scene==NULL)
  {
    vtkWarningMacro("Failed to create display node for "<<(volumeNode->GetID()?volumeNode->GetID():"unknown volume node")<<": scene is invalid");
    return;
  }
  // If the input is a 3-component image then we assume it is a color image,
  // and we display it in true color. For true color display we need to use
  // a vtkMRMLVectorVolumeDisplayNode.
  vtkSmartPointer<vtkMRMLVolumeDisplayNode> displayNode;
  displayNode = vtkSmartPointer<vtkMRMLVectorVolumeDisplayNode>::New();
  scene->AddNode(displayNode);
  displayNode->SetDefaultColorMap();
  
  volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  
}

//---------------------------------------------------------------------------
int vtkIGTLToMRMLVideo::IGTLToVTKScalarType(int igtlType)
{
  switch (igtlType)
  {
    case igtl::VideoMessage::TYPE_INT8: return VTK_CHAR;
    case igtl::VideoMessage::TYPE_UINT8: return VTK_UNSIGNED_CHAR;
    case igtl::VideoMessage::TYPE_INT16: return VTK_SHORT;
    case igtl::VideoMessage::TYPE_UINT16: return VTK_UNSIGNED_SHORT;
    case igtl::VideoMessage::TYPE_INT32: return VTK_UNSIGNED_LONG;
    case igtl::VideoMessage::TYPE_UINT32: return VTK_UNSIGNED_LONG;
    default:
      vtkErrorMacro ("Invalid IGTL scalar Type: "<<igtlType);
      return VTK_VOID;
  }
}