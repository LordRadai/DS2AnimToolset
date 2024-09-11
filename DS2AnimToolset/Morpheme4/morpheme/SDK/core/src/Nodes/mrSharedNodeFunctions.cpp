// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Functions for use by all nodes.
//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceNULL(
  const NodeDef* NMP_UNUSED(node),
  Network*       NMP_UNUSED(net))
{
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceNoChildren(
  const NodeDef* node,
  Network*       net)
{
  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceWithChildren(
  const NodeDef* node,
  Network*       net)
{
  NET_LOG_MESSAGE(99, "Deleting default node %s\n", node->getName());
  // Recurse to our children first.
  NMP_ASSERT(net->getNetworkDef());
  for (uint32_t i = 0; i < node->getNumChildNodes(); ++i)
  {
    if(node->getChildNodeID(i) != INVALID_NODE_ID)
    {
      net->deleteNodeInstance(node->getChildNodeID(i));
    }
  }

  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
// Default node update connections functions.
//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsNULL(
  NodeDef* node,
  Network* NMP_UNUSED(net))
{
  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);

  // Recurse to children.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildrenInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
  {
    net->updateInputCPConnection(node->getInputCPConnection(i), animSet);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildrenOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSet);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren1CompulsoryManyOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the one compulsory control parameter, which must be the first one.
  uint32_t numInputCPConnections = node->getNumInputCPConnections();
  NMP_ASSERT(numInputCPConnections > 0);
  net->updateInputCPConnection(node->getInputCPConnection(0), animSetIndex);

  // Update the other control parameters - all optional
  for (PinIndex i = 1; i < numInputCPConnections; ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSetIndex);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSetIndex);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren2CompulsoryManyOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the compulsory control parameters, which must be the first ones.
  uint32_t numInputCPConnections = node->getNumInputCPConnections();
  NMP_ASSERT(numInputCPConnections > 1);
  net->updateInputCPConnection(node->getInputCPConnection(0), animSetIndex);
  net->updateInputCPConnection(node->getInputCPConnection(1), animSetIndex);

  // Update the other control parameters - all optional
  for (PinIndex i = 2; i < numInputCPConnections; ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSetIndex);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSetIndex);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child1InputCP(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateInputCPConnection(node->getInputCPConnection(0), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child1OptionalInputCP(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateOptionalInputCPConnection(node->getInputCPConnection(0), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child2InputCPs(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 2);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateInputCPConnection(node->getInputCPConnection(0), animSet);
  net->updateInputCPConnection(node->getInputCPConnection(1), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
// Default node init instance functions.
// These functions are called only when the network is instantiated.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceInvalidateAllChildrenInitPinAttrib(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
    connections->m_activeChildNodeIDs[iChild] = INVALID_NODE_ID;

  connections->m_numActiveChildNodes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceInvalidateAllChildren(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
    connections->m_activeChildNodeIDs[iChild] = INVALID_NODE_ID;

  connections->m_numActiveChildNodes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateFloatOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateIntOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateIntOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateVector3OutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateVector3OutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateBoolOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateBoolOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// An empty OutputCP function.
//  Every node that can emit requests must have an extra output CP function 
//  that actually deals with the update of emitted requests. 
//  The registered function can be empty if the requests are actually sent elsewhere.
//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeNullEmitMessageOutputCPUpdate(
  NodeDef* NMP_UNUSED(node),
  PinIndex NMP_UNUSED(outputCPPinIndex), /// The output pin we have been asked to update.
  Network* NMP_UNUSED(net))
{
  return NULL;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
