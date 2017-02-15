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

// VTK includes
#include <vtkStdString.h>
#include <vtkImageData.h>

class vtkMRMLVectorVolumeNode;

class  VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT vtkMRMLBitStreamNode : public vtkMRMLNode
{
public:
  
  static vtkMRMLBitStreamNode *New();
  vtkTypeMacro(vtkMRMLBitStreamNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual vtkMRMLNode* CreateNodeInstance();
  
  ///
  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);
  
  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);
  
  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);
  
  virtual void SetScene(vtkMRMLScene *scene);

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "BitStream";};
  
  ///
  /// Set codec name
  vtkSetMacro(CodecName, char *);
  vtkGetMacro(CodecName, char *);
  
  void SetVectorVolumeNode(vtkMRMLVectorVolumeNode* imageData);
  
  vtkMRMLVectorVolumeNode* GetVectorVolumeNode();

  void DecodeMessageStream(igtl::MessageBase::Pointer buffer)
  {
    int oldModified = vectorVolumeNode->StartModify();
    this->vectorVolumeNode->SetName("MacCamera5"); // strip the _BitStream_ characters
    videoDecoder->SetVideoDecoderName(0, (char*)"MacCamera5");
    if(this->MessageBufferValid)
      videoDecoder->IGTLToMRML(this->MessageBuffer, vectorVolumeNode);
    this->vectorVolumeNode->EndModify(oldModified);
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
  
  bool GetMessageValid()
  {return MessageBufferValid;};
  
protected:
  vtkMRMLBitStreamNode();
  ~vtkMRMLBitStreamNode();
  vtkMRMLBitStreamNode(const vtkMRMLBitStreamNode&);
  void operator=(const vtkMRMLBitStreamNode&);
  
  char* CodecName;
  vtkMRMLVectorVolumeNode * vectorVolumeNode;
  igtl::MessageBase::Pointer MessageBuffer;
  bool MessageBufferValid;
  
  vtkIGTLToMRMLVideo* videoDecoder;
  
};

#endif
