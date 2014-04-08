//
//  CharacterHistoryNodeMonitor.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 10/16/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#include "CharacterHistoryNodeMonitor.h"
#include "DagNode.h"
#include "Model.h"
#include "Monitor.h"
#include "RbException.h"

#include <sstream>

using namespace RevBayesCore;

/* Constructor */
CharacterHistoryNodeMonitor::CharacterHistoryNodeMonitor(TypedDagNode<TimeTree>* t,  std::vector<StochasticNode<BranchHistory>* > bh, int g, const std::string &fname, const std::string &del, bool pp, bool l, bool pr, bool ap, bool sm, bool sr) : Monitor(g,t), outStream(), tree( t ), branchHistories(bh), filename( fname ), separator( del ), posterior( pp ), prior( pr ), likelihood( l ), append(ap), showMetadata(sm), showRates(sr) {
    
    nodes.push_back(t);
    for (size_t i = 0; i < branchHistories.size(); i++)
        nodes.push_back(branchHistories[i]);
    
}

CharacterHistoryNodeMonitor::CharacterHistoryNodeMonitor(const CharacterHistoryNodeMonitor &m) : Monitor( m ), outStream( ), tree( m.tree ), branchHistories( m.branchHistories), nodeVariables( m.nodeVariables ), showMetadata(m.showMetadata), showRates(m.showRates) {
    
    filename    = m.filename;
    separator   = m.separator;
    prior       = m.prior;
    posterior   = m.posterior;
    likelihood  = m.likelihood;
    append      = m.append;
}


/* Clone the object */
CharacterHistoryNodeMonitor* CharacterHistoryNodeMonitor::clone(void) const {
    
    return new CharacterHistoryNodeMonitor(*this);
}


void CharacterHistoryNodeMonitor::closeStream() {
    outStream.close();
}


std::string CharacterHistoryNodeMonitor::buildExtendedNewick( void ) {
    //tree->getValue().getRoot().setNewickNeedsRefreshing(true);
    std::string newick = buildExtendedNewick( &tree->getValue().getRoot() );
    return newick;
}


std::string CharacterHistoryNodeMonitor::buildCharacterHistoryString(TopologyNode* n, std::string brEnd)
{
    
    BranchHistory bh = branchHistories[n->getIndex()]->getValue();
    std::vector<CharacterEvent*> characters;
    if (brEnd=="child")
        characters = bh.getChildCharacters();
    else if (brEnd=="parent")
        characters = bh.getParentCharacters();
    
    std::stringstream ss;
    for (size_t i = 0; i < characters.size(); i++)
    {
        if (i != 0)
            ss << ",";
        ss << characters[i]->getState();
    }
    
    return ss.str();
}

/* Build newick string */
std::string CharacterHistoryNodeMonitor::buildExtendedNewick( TopologyNode* n ) {
    // create the newick string
    std::stringstream o;
    
    // extended data is only found on admixture nodes
    std::string additionalInfo = "";
    
    // loop over admixture nodes per branch
    std::stringstream characterStream;
    
    double br = 1.0;
    
    if (showMetadata)
    {
        characterStream << "[";
        
        // character history
        characterStream << "&ch={" << buildCharacterHistoryString(n,"child") << "}";
        
        // ... whatever else
        characterStream << "]";
        
        additionalInfo = characterStream.str();
    }

    // test whether this is a internal or external node
    if (n->isTip()) {
        // this is a tip so we just return the name of the node
        o << n->getName() << additionalInfo << ":" << n->getBranchLength();
    }
    
    else {
        o << "(";
        for (size_t i=0; i<(n->getNumberOfChildren()-1); i++) {
            o << buildExtendedNewick( &n->getChild(i) ) << ",";
        }
        o << buildExtendedNewick( &n->getChild(n->getNumberOfChildren()-1) ) << ")" << additionalInfo << ":" << n->getBranchLength() * br;
    }
    
    return o.str();
}


/** Monitor value at generation gen */
void CharacterHistoryNodeMonitor::monitor(long gen) {
    
    // get the printing frequency
    int samplingFrequency = printgen;
    
    if (gen % samplingFrequency == 0) {
        // print the iteration number first
        outStream << gen;
        
        if ( posterior ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                pp += (*it)->getLnProbability();
            }
            outStream << pp;
        }
        
        if ( likelihood ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                if ( (*it)->isClamped() ) {
                    pp += (*it)->getLnProbability();
                }
            }
            outStream << pp;
        }
        
        if ( prior ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                if ( !(*it)->isClamped() ) {
                    pp += (*it)->getLnProbability();
                }
            }
            outStream << pp;
        }
        
        // add a separator before the tree
        outStream << separator;
        
        std::string en = buildExtendedNewick();
        //std::cout << "\n" << en << "\n\n";
        
        outStream << en;
        
        outStream << std::endl;
        
    }
}


/** open the file stream for printing */
void CharacterHistoryNodeMonitor::openStream(void) {
    
    // open the stream to the file
    if (append)
        outStream.open( filename.c_str(), std::fstream::out | std::fstream::app);
    else
        outStream.open( filename.c_str(), std::fstream::out);
}

/** Print header for monitored values */
void CharacterHistoryNodeMonitor::printHeader() {
    
    // print one column for the iteration number
    outStream << "Iteration";
    
    if ( posterior ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Posterior";
    }
    
    if ( likelihood ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Likelihood";
    }
    
    if ( prior ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Prior";
    }
    
    // add a separator tree
    outStream << separator << "Tree";
    
    // end line of stream
    outStream << std::endl;
}


void CharacterHistoryNodeMonitor::swapNode(DagNode *oldN, DagNode *newN) {

    bool found = false;
    if ( oldN == tree )
    {
        tree = static_cast< TypedDagNode< TimeTree > *>( newN );
        found = true;
    }
    for (size_t i = 0; i < branchHistories.size(); i++)
    {
        if (oldN == branchHistories[i])
        {
            branchHistories[i] = static_cast<StochasticNode<BranchHistory>* >(newN);
            found = true;
        }
    }
    
    /*
    if (found == false)
    {
        // error catching
        if ( nodeVariables.find(oldN) == nodeVariables.end() ) {
            throw RbException("Cannot replace DAG node in this monitor because the monitor doesn't hold this DAG node.");
        }
        
        nodeVariables.erase( oldN );
        nodeVariables.insert( newN );
    }
    */
    
    // delegate to base class
    Monitor::swapNode(oldN, newN);
}
