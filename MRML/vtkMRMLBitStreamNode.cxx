#include "vtkObjectFactory.h"
#include "vtkMRMLBitStreamNode.h"
#include "vtkXMLUtilities.h"
#include "vtkMRMLScene.h"
// VTK includes
#include <vtkNew.h>
#include <vtkCollection.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLBitStreamNode);

//-----------------------------------------------------------------------------
vtkMRMLBitStreamNode::vtkMRMLBitStreamNode()
{
  this->CodecName = new char[10];
  
  memcpy(this->CodecName, (char *)"H264", 4);
  this->BitStream = NULL;
  this->BitStreamLength = 0;
  vectorVolumeNode = vtkMRMLVectorVolumeNode::New();
  videoDecoder = vtkIGTLToMRMLVideo::New();
}

//-----------------------------------------------------------------------------
vtkMRMLBitStreamNode::~vtkMRMLBitStreamNode()
{
  if(this->CodecName)
  {
    delete this->CodecName;
    this->CodecName  = NULL;
  }
  if(this->BitStream)
  {
    delete this->BitStream;
    this->BitStream = NULL;
  }
}

//-----------------------------------------------------------
void vtkMRMLBitStreamNode::SetScene(vtkMRMLScene *scene)
{
  this->Superclass::SetScene(scene);
  vectorVolumeNode->SetScene(scene);
}

void vtkMRMLBitStreamNode::DecodeBitStream(char* bitstream, int length)
{
  int oldModified = vectorVolumeNode->StartModify();
  this->SetBitStream(bitstream, length);
  igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
  videoMsg->InitPack();
  videoMsg->SetDeviceName("MacCamera5_BitStream"); //trim the bit stream node name
  videoMsg->SetBitStreamSize(length);
  videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  videoMsg->AllocateBuffer();
  memcpy(videoMsg->GetPackFragmentPointer(2),bitstream, length);
  videoMsg->Pack();
  igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
  header->AllocateBuffer();
  memcpy(header->GetPackPointer(), videoMsg->GetPackPointer(),IGTL_HEADER_SIZE);
  header->Unpack();
  igtl::MessageBase* buffer = igtl::MessageBase::New();
  //buffer->AllocatePack();
  //buffer->SetMessageHeader(header);
  buffer->AllocateBuffer();
  buffer->Copy(videoMsg);
  this->vectorVolumeNode->SetName("MacCamera5"); // strip the _BitStream_ characters
  videoDecoder->IGTLToMRML(buffer, vectorVolumeNode);
  this->vectorVolumeNode->EndModify(oldModified);
}

void vtkMRMLBitStreamNode::SetVectorVolumeNode(vtkMRMLVectorVolumeNode* volumeNode)
{
  this->vectorVolumeNode = volumeNode;
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

