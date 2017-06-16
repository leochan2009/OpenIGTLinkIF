#ifndef __vtkMRMLBitStreamNode_h
#define __vtkMRMLBitStreamNode_h

// OpenIGTLinkIF MRML includes
#include "vtkIGTLToMRMLBase.h"
#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// MRML includes
#include <vtkMRMLStorableNode.h>
#include "vtkMRMLVectorVolumeDisplayNode.h"
#include "vtkMRMLVectorVolumeNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkIGTLToMRMLVideo.h"
#include "vtkIGTLToMRMLImage.h"

// VTK includes
#include <vtkStdString.h>
#include <vtkImageData.h>

static std::string SEQ_BITSTREAM_POSTFIX = "_BitStream";

class  VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkMRMLBitStreamNode : public vtkMRMLNode
{
public:
  
  static vtkMRMLBitStreamNode *New();
  vtkTypeMacro(vtkMRMLBitStreamNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual vtkMRMLNode* CreateNodeInstance();
  
  virtual void ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
  {
    this->vtkMRMLNode::ProcessMRMLEvents( caller, event, callData );
    vtkMRMLNode* modifiedNode = vtkMRMLNode::SafeDownCast(caller);
    if (modifiedNode == NULL)
    {
      // we are only interested in proxy node modified events
      return;
    }
    this->InvokeCustomModifiedEvent(event, this);
  }

  ///
  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);
  
  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);
  
  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "BitStream";};
  
  
  void SetVectorVolumeNode(vtkMRMLVectorVolumeNode* imageData);
  
  void SetVideoMessageConverter(vtkIGTLToMRMLVideo* inConverter)
  {
    this->converter = inConverter;
  };
  
  vtkMRMLVectorVolumeNode* GetVectorVolumeNode();
  
  void SetUpMRMLNodeAndConverter(const char* name );

  void DecodeMessageStream(igtl::MessageBase::Pointer buffer)
  {
    if(vectorVolumeNode)
    {
      converter->IGTLToMRML(buffer, vectorVolumeNode, true);
    }
  };
  
  void SetKeyFrameDecodedFlag(bool flag)
  {
    this->isKeyFrameDecoded = flag;
  };
  
  bool GetKeyFrameDecodedFlag()
  {
    return this->isKeyFrameDecoded;
  };
  
  void SetMessageStream(igtl::MessageBase::Pointer buffer)
  {
    this->MessageBuffer->SetMessageHeader(buffer);
    this->MessageBuffer->AllocateBuffer();
    memcpy(this->MessageBuffer->GetPackPointer(), buffer->GetPackPointer(), buffer->GetPackSize());
    this->MessageBufferValid = true;
  };
  
  igtl::MessageBase::Pointer GetMessageStreamBuffer()
  {
    return MessageBuffer;
  };
  
  igtl::ImageMessage::Pointer GetImageMessageBuffer()
  {
    return ImageMessageBuffer;
  };
  
  void SetMessageValid(bool value)
  {
    MessageBufferValid = value
    ;
  };
  
  void SetVectorVolumeName(const char* name)
  {
    this->vectorVolumeNode->SetName(name);
  };
  
  
  bool GetMessageValid()
  {
    return MessageBufferValid;
  };
  
protected:
  vtkMRMLBitStreamNode();
  ~vtkMRMLBitStreamNode();
  vtkMRMLBitStreamNode(const vtkMRMLBitStreamNode&);
  void operator=(const vtkMRMLBitStreamNode&);
  
  vtkMRMLVectorVolumeNode * vectorVolumeNode;
  igtl::MessageBase::Pointer MessageBuffer;
  igtl::ImageMessage::Pointer ImageMessageBuffer;
  bool MessageBufferValid;
  
  vtkIGTLToMRMLVideo* converter;
  
  bool isKeyFrameDecoded;
  
};

#endif
