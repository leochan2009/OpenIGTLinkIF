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
  
  void DecodeBitStream(char* bitstream, int length);
  
  void SetBitStream(char* bitstream, int length)
  {
    if(BitStream)
    {
      delete BitStream;
    }
    this->BitStream = new char[length];
    this->BitStreamLength = length;
    memcpy(BitStream, bitstream, length);
  }
  
  char* GetBitStream()
  {
    return BitStream;
  };
  
  int GetBitStreamLength()
  {
    return BitStreamLength;
  };
  
protected:
  vtkMRMLBitStreamNode();
  ~vtkMRMLBitStreamNode();
  vtkMRMLBitStreamNode(const vtkMRMLBitStreamNode&);
  void operator=(const vtkMRMLBitStreamNode&);
  
  char* CodecName;
  char* BitStream;
  int BitStreamLength;
  vtkMRMLVectorVolumeNode * vectorVolumeNode;
  
  vtkIGTLToMRMLVideo* videoDecoder;
  
};

#endif
