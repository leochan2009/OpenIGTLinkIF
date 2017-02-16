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
  this->CodecName = new char[10];
  memcpy(this->CodecName, (char *)"H264", 4);
  vectorVolumeNode = NULL;
  videoDecoder = NULL;
  
  MessageBuffer = igtl::MessageBase::New();
  MessageBuffer->InitPack();
  MessageBufferValid = false;
}

//-----------------------------------------------------------------------------
vtkMRMLBitStreamNode::~vtkMRMLBitStreamNode()
{
  if(this->CodecName)
  {
    delete this->CodecName;
    this->CodecName  = NULL;
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
void vtkMRMLBitStreamNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  Superclass::ReadXMLAttributes(atts);
  
  const char* attName;
  const char* attValue;
  while (*atts != NULL)
  {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "codecname"))
    {
      this->SetCodecName((char*)attValue);
    }
  }
  
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
  
  vtkIndent indent(nIndent);
  
  of << indent << " codecname=\"";
  if (this->GetCodecName()!=NULL)
  {
    // Write to XML, encoding special characters, such as " ' \ < > &
    vtkXMLUtilities::EncodeString(this->GetCodecName(), VTK_ENCODING_NONE, of, VTK_ENCODING_NONE, 1 /* encode special characters */ );
  }
  of << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLBitStreamNode *node = vtkMRMLBitStreamNode::SafeDownCast(anode);
  
  this->SetCodecName(node->GetCodecName());
  
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLBitStreamNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  
  os << "Codec Name: " << ( (this->GetCodecName()) ? this->GetCodecName() : "(none)" ) << "\n";
}

