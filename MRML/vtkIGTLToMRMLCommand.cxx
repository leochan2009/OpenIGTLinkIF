/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLCommand.cxx $
  Date:      $Date: 2009-10-05 17:37:20 -0400 (Mon, 05 Oct 2009) $
  Version:   $Revision: 10577 $

==========================================================================*/

#include "vtkIGTLToMRMLCommand.h"
#include "vtkMRMLIGTLQueryNode.h"
#include "vtkMRMLTextNode.h"
#include "igtlCommandMessage.h"

#include "vtkMRMLIGTLQueryNode.h"

// VTK includes
#include "vtkCommand.h"
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkIGTLToMRMLCommand);


//---------------------------------------------------------------------------
vtkIGTLToMRMLCommand::vtkIGTLToMRMLCommand() {}


//---------------------------------------------------------------------------
vtkIGTLToMRMLCommand::~vtkIGTLToMRMLCommand() {}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}


//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLCommand::CreateNewNode(vtkMRMLScene* scene, const char* name)
{
  vtkSmartPointer< vtkMRMLTextNode > textNode = vtkSmartPointer< vtkMRMLTextNode >::New();
  textNode->SetName( name );
  textNode->SetDescription( "Created by OpenIGTLinkIF module" );
  
  scene->AddNode( textNode );
  
  return textNode;
}


//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLCommand::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);

  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLCommand::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{
  vtkIGTLToMRMLBase::IGTLToMRML( buffer, node );
  
  if ( node == NULL )
  {
    return 0;
  }
  
  // Create message buffer to receive data.
  
  igtl::CommandMessage::Pointer commandMessage;
  commandMessage = igtl::CommandMessage::New();
  commandMessage->Copy( buffer );
  
  int c = commandMessage->Unpack( this->CheckCRC );
  
  if ( ! ( c & igtl::MessageHeader::UNPACK_BODY ) )
  {
    vtkErrorMacro( "Incoming IGTL string message failed CRC check!" );
    return 0;
  }
  
  vtkMRMLTextNode* textNode = vtkMRMLTextNode::SafeDownCast( node );
  if ( textNode == NULL )
  {
    vtkErrorMacro( "Could not convert node to TextNode." );
    return 0;
  }
  
  int oldModify = textNode->StartModify();
  textNode->SetText(commandMessage->GetCommandContent().c_str());
  textNode->SetEncoding(commandMessage->GetContentEncoding());
  textNode->EndModify(oldModify);
  
  return 1;

}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLCommand::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if ( mrmlNode == NULL )
  {
    vtkErrorMacro("vtkIGTLToMRMLString::MRMLToIGTL failed: invalid input MRML node");
    return 0;
  }
  
  const char* deviceName = NULL;
  const char* text = NULL;
  int encoding = vtkMRMLTextNode::ENCODING_US_ASCII;
  
  vtkMRMLTextNode* textNode = vtkMRMLTextNode::SafeDownCast( mrmlNode );
  vtkMRMLIGTLQueryNode* queryNode = vtkMRMLIGTLQueryNode::SafeDownCast( mrmlNode );
  if ( textNode != NULL && event == vtkCommand::ModifiedEvent)
  {
    deviceName = textNode->GetName();
    text = textNode->GetText();
    encoding = textNode->GetEncoding();
  }
  else if ( queryNode != NULL )
  {
    // Special case for STRING command handling.
    // The command is a regular STRING message with special device name (CMD_...).
    // Note that the query node has a name that matches the response node name (ACK_...),
    // as it is for detecting the arrival of the response.
    deviceName = queryNode->GetAttribute("CommandDeviceName");
    text = queryNode->GetAttribute("CommandString");
  }
  
  if (deviceName!=NULL && text!=NULL)
  {
    if (this->commandMsg.GetPointer()==NULL)
    {
      this->commandMsg = igtl::CommandMessage::New();
    }
    this->commandMsg->SetDeviceName( deviceName );
    this->commandMsg->SetCommandContent(text );
    this->commandMsg->SetContentEncoding(encoding );
    this->commandMsg->Pack();
    *size = this->commandMsg->GetPackSize();
    *igtlMsg = (void*)this->commandMsg->GetPackPointer();
    return 1;
  }
  
  return 0;
}



