#include "vtkObjectFactory.h"
#include "vtkMRMLBitStreamNode.h"
#include "vtkXMLUtilities.h"
#include "vtkMRMLScene.h"
// VTK includes
#include <vtkNew.h>
#include <vtkCollection.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLBitStreamNode);

//-----------------------------------------------------------------------------
vtkMRMLBitStreamNode::vtkMRMLBitStreamNode()
{
  vectorVolumeNode = NULL;
  converter = NULL;
  
  MessageBuffer = igtl::MessageBase::New();
  MessageBuffer->InitPack();
  MessageBufferValid = false;
  ImageMessageBuffer = igtl::ImageMessage::New();
  ImageMessageBuffer->InitPack();
  MessageBuffer->InitPack();
}

//-----------------------------------------------------------------------------
vtkMRMLBitStreamNode::~vtkMRMLBitStreamNode()
{
}

void vtkMRMLBitStreamNode::SetUpMRMLNodeAndConverter(const char* name)
{
  vtkMRMLScene* scene = this->GetScene();
  if(scene)
  {
    vtkCollection* collection =  scene->GetNodesByClassByName("vtkMRMLVectorVolumeNode",name);
    int nCol = collection->GetNumberOfItems();
    if (nCol > 0)
    {
      for (int i = 0; i < nCol; i ++)
      {
        this->GetScene()->RemoveNode(vtkMRMLNode::SafeDownCast(collection->GetItemAsObject(i)));
      }
    }
    vectorVolumeNode = vtkMRMLVectorVolumeNode::New();
    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    vectorVolumeNode->SetAndObserveImageData(image);
    scene->SaveStateForUndo();
    vtkDebugMacro("Setting scene info");
    scene->AddNode(vectorVolumeNode);
    vectorVolumeNode->SetScene(scene);
    vectorVolumeNode->SetDescription("Received by OpenIGTLink");
    vectorVolumeNode->SetName(name);
    std::string nodeName(name);
    nodeName.append(SEQ_BITSTREAM_POSTFIX);
    this->SetName(nodeName.c_str());
    vectorVolumeNode->AddObserver(vtkMRMLVolumeNode::ImageDataModifiedEvent, this, &vtkMRMLBitStreamNode::ProcessMRMLEvents);
    //------
    //converter initialization
    converter = vtkIGTLToMRMLVideo::New();
    int i = 0;
    for (i = 0; i< VideoThreadMaxNumber; i++)
    {
      if (converter->VideoStreamDecoder[i]->GetDeviceName().compare("")==0)
      {
        converter->VideoStreamDecoder[i]->SetDeviceName(name);
        break;
      }
    }
    converter->SetDefaultDisplayNode(vectorVolumeNode,3);
    converter->CenterImage(vectorVolumeNode);
    //-------
  }
}


void vtkMRMLBitStreamNode::SetVectorVolumeNode(vtkMRMLVectorVolumeNode* volumeNode)
{
  this->vectorVolumeNode = vtkMRMLVectorVolumeNode::SafeDownCast(volumeNode);
}

vtkMRMLVectorVolumeNode* vtkMRMLBitStreamNode::GetVectorVolumeNode()
{
  return this->vectorVolumeNode;
}


//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  
  Superclass::ReadXMLAttributes(atts);
  
  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

