#include "RlAbstractHomologousDiscreteCharacterData.h"
#include "RlAbstractDiscreteTaxonData.h"
#include "RlDistanceMatrix.h"
#include "ArgumentRule.h"
#include "RlMatrixReal.h"
#include "MemberProcedure.h"
#include "ModelVector.h"
#include "Natural.h"
#include "RlBoolean.h"
#include "Probability.h"
#include "RlString.h"
#include "RlDiscreteTaxonData.h"
#include "RlSimplex.h"
#include "RbBitSet.h"


using namespace RevLanguage;

AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData(void) : ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>(),
HomologousCharacterData( )
{
    
    initMethods();
    
}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( const RevBayesCore::AbstractHomologousDiscreteCharacterData &d) : ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>( d.clone() ),
HomologousCharacterData( )
{
    
    initMethods();
    
}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( RevBayesCore::AbstractHomologousDiscreteCharacterData *d) : ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>( d ),
HomologousCharacterData( )
{
    
    initMethods();
    
}


AbstractHomologousDiscreteCharacterData::AbstractHomologousDiscreteCharacterData( RevBayesCore::TypedDagNode<RevBayesCore::AbstractHomologousDiscreteCharacterData> *d) : ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>( d ),
HomologousCharacterData( )
{
    
    initMethods();
    
}



AbstractHomologousDiscreteCharacterData::~AbstractHomologousDiscreteCharacterData()
{
    
    
}



void AbstractHomologousDiscreteCharacterData::concatenate(const RevObject &d, std::string type) const
{
    const AbstractHomologousDiscreteCharacterData* tmp = dynamic_cast<const AbstractHomologousDiscreteCharacterData*>( &d );
    if ( tmp != NULL )
    {
        concatenate( *tmp, type );
    }
    else
    {
        throw RbException("Cannot add an object of type '" + d.getType() + "' to a character data object.");
    }
    
}


void AbstractHomologousDiscreteCharacterData::concatenate(const AbstractHomologousDiscreteCharacterData &d, std::string type) const
{
    
        // we need to make this a constant DAG node so that we can actually modify the value
        // otherwise the value might be overwritten again, e.g., if this is a deterministic node.
        //    clone_obj->makeConstantValue();
    
        // now concatenate
    getDagNode()->getValue().concatenate( d.getValue(), type );
    
}



AbstractHomologousDiscreteCharacterData* AbstractHomologousDiscreteCharacterData::clone() const
{
    return new AbstractHomologousDiscreteCharacterData( *this );
}


/* Map calls to member methods */
RevPtr<RevVariable> AbstractHomologousDiscreteCharacterData::executeMethod(std::string const &name, const std::vector<Argument> &args, bool &found)
{
    
    RevPtr<RevVariable> retVal = dynamic_cast<RevMemberObject *>( dag_node )->executeMethod(name, args, found);
    
    if ( found == true )
    {
        return retVal;
    }
    
    if ( this->getDagNode() != NULL )
    {
            // set the internal value pointer
            //        setCharacterDataObject( &this->getDagNode()->getValue() );
    }
    
    retVal = executeCharacterDataMethod(name, args, found, &this->getValue());
    
    if ( found == true )
    {
        return retVal;
    }
    else if (name == "[]")
    {
        found = true;
        
            // get the member with give index
        const Natural& index = static_cast<const Natural&>( args[0].getVariable()->getRevObject() );
        
        if (this->dag_node->getValue().getNumberOfTaxa() < (size_t)(index.getValue()) )
        {
            throw RbException("Index out of bounds in []");
        }
        
        const RevBayesCore::AbstractDiscreteTaxonData& element = dag_node->getValue().getTaxonData(size_t(index.getValue()) - 1);
        
        return new RevVariable( new AbstractDiscreteTaxonData( element.clone() ) );
    }
    else if (name == "computeMultinomialProfileLikelihood")
    {
        found = true;
        
        double lnl = this->dag_node->getValue().computeMultinomialProfileLikelihood();
        
        return new RevVariable( new Real(lnl) );
    }
    else if (name == "computeStateFrequencies")
    {
        found = true;
        
        RevBayesCore::MatrixReal sf = this->dag_node->getValue().computeStateFrequencies();
        
        return new RevVariable( new MatrixReal(sf) );
    }
    else if ( name == "expandCharacters" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        int n = static_cast<const Natural&>( argument ).getValue();
        
        RevBayesCore::AbstractHomologousDiscreteCharacterData *trans_data = this->dag_node->getValue().expandCharacters( n );
        
        return new RevVariable( new AbstractHomologousDiscreteCharacterData(trans_data) );
    }
    else if (name == "getEmpiricalBaseFrequencies")
    {
        found = true;
        
        std::vector<double> ebf = this->dag_node->getValue().getEmpiricalBaseFrequencies();
        
        return new RevVariable( new Simplex(ebf) );
    }
    else if (name == "getNumInvariantSites")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t n = this->dag_node->getValue().getNumberOfInvariantSites( excl );
        
        return new RevVariable( new Natural(n) );
    }
    else if ( name == "getStateDescriptions" )
    {
        found = true;
        
        std::vector<std::string> descriptions = this->dag_node->getValue().getTaxonData(0).getCharacter(0).getStateDescriptions();
        
        return new RevVariable( new ModelVector<RlString>(descriptions) );
    }
    else if (name == "isHomologous")
    {
        found = true;
        
        bool ih = this->dag_node->getValue().isHomologyEstablished();
        
        return new RevVariable( new RlBoolean(ih) );
    }
    else if ( name == "maxGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double max_gc = this->dag_node->getValue().maxGcContent( excl );
        
        return new RevVariable( new Probability(max_gc) );
    }
    else if ( name == "maxInvariableBlockLength" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_inv = this->dag_node->getValue().maxInvariableBlockLength( excl );
        
        return new RevVariable( new Natural(max_inv) );
    }
    else if ( name == "maxPairwiseDifference" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_pd = this->dag_node->getValue().getMaxPaiwiseSequenceDifference( excl );
        
        return new RevVariable( new Natural(max_pd) );
    }
    else if ( name == "maxVariableBlockLength" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t max_var = this->dag_node->getValue().maxVariableBlockLength( excl );
        
        return new RevVariable( new Natural(max_var) );
    }
    else if ( name == "meanGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double mean_gc = this->dag_node->getValue().meanGcContent( excl );
        
        return new RevVariable( new Probability(mean_gc) );
    }
    else if ( name == "meanGcContentByCodonPosition" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        
        const RevObject& excl_argument = args[1].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( excl_argument ).getValue();
        
        double mean_gc = this->dag_node->getValue().meanGcContentByCodon( n, excl );
        
        return new RevVariable( new Probability(mean_gc) );
    }
    else if ( name == "minGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double min_gc = this->dag_node->getValue().minGcContent( excl );
        
        return new RevVariable( new Probability(min_gc) );
    }
    else if ( name == "minPairwiseDifference" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t min_pd = this->dag_node->getValue().getMinPaiwiseSequenceDifference( excl );
        
        return new RevVariable( new Natural(min_pd) );
    }
    else if ( name == "getPairwiseDifference" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        RevBayesCore::DistanceMatrix pd = this->dag_node->getValue().getPaiwiseSequenceDifference( excl );
        
        return new RevVariable( new DistanceMatrix(pd) );
    }
    else if ( name == "numInvariableBlocks" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        size_t num_blocks = this->dag_node->getValue().numInvariableSiteBlocks( excl );
        
        return new RevVariable( new Natural(num_blocks) );
    }
    else if ( name == "numTaxaMissingSequence" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        double percentage = static_cast<const Probability&>( argument ).getValue();
        
        size_t num_taxa = this->dag_node->getValue().numberTaxaMissingSequence( percentage );
        
        return new RevVariable( new Natural(num_taxa) );
    }
    else if (name == "setCodonPartition")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        RevBayesCore::AbstractHomologousDiscreteCharacterData &v = dag_node->getValue();
        size_t nChars = v.getNumberOfCharacters();
        
            // e.g. data.setCodonPartition(sites=v(3))
        if ( argument.isType( Natural::getClassTypeSpec() ) )
        {
            size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
            size_t i = 0; // index of included characters
            for (size_t j = 0; j < nChars; j++)
            {
                    // only set codon partition for previously included characters
                if ( !v.isCharacterExcluded(j) )
                {
                    if ( i % 3 == (n-1) )
                    {
                        v.includeCharacter(j);
                    }
                    else
                    {
                        v.excludeCharacter(j);
                    }
                    i++;
                }
            }
            
        }
        
            // e.g. data.setCodonPartition(sites=v(1,2))
        else if ( argument.isType( ModelVector<Natural>::getClassTypeSpec() ) )
        {
            const ModelVector<Natural>& x = static_cast<const ModelVector<Natural>&>( argument );
            if (x.size() == 0)
            {
                return NULL;
            }
            
            size_t i = 0; // index of included characters
            for (size_t j = 0; j < nChars; j++)
            {
                    // only set codon partition for previously included characters
                if ( !v.isCharacterExcluded(j) )
                {
                    bool included_codon = false;
                    for (size_t k = 0; k < x.size(); k++)
                    {
                        size_t n = x[k];
                        if ( i % 3 == (n-1) )
                        {
                            v.includeCharacter(j);
                            included_codon = true;
                            break;
                        }
                    }
                    if ( !included_codon )
                    {
                        v.excludeCharacter(j);
                    }
                    i++;
                }
            }
        }
        return NULL;
    }
    else if (name == "setNumStatesPartition")
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        RevBayesCore::AbstractHomologousDiscreteCharacterData &v = dag_node->getValue();
        size_t nChars = v.getNumberOfCharacters();
        size_t nTaxa = v.getNumberOfTaxa();
        
            // e.g. data.setNumStatesPartition(2)
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        for (size_t i = 0; i < nChars; i++)
        {
            int max = 0;
            for (size_t j = 0; j < nTaxa; j++)
            {
                const RevBayesCore::AbstractDiscreteTaxonData& td = v.getTaxonData(j);
                if ( td.getCharacter(i).isMissingState() == false && td.getCharacter(i).isGapState() == false)
                {
                    if(td.getCharacter(i).getNumberObservedStates() > 1)
                    {
                        const RevBayesCore::RbBitSet& state = td.getCharacter(i).getState();
                        for(size_t k = 0; k < state.size(); k++)
                        {
                            if(state.isSet(k) && k +1 > max)
                            {
                                max = k+1;
                            }
                        }
                    }
                    else
                    {
                        int k = int(td.getCharacter(i).getStateIndex()) + 1;
                        if (k > max)
                        {
                            max = k;
                        }
                    }
                }
            }
            
            if (max == n)
            {
                v.includeCharacter(i);
            }
            else
            {
                v.excludeCharacter(i);
            }
            
        }
        return NULL;
    }
    else if ( name == "translateCharacters" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        const std::string &type = static_cast<const RlString&>( argument ).getValue();
        
        RevBayesCore::AbstractHomologousDiscreteCharacterData *trans_data = this->dag_node->getValue().translateCharacters( type );
        
        return new RevVariable( new AbstractHomologousDiscreteCharacterData(trans_data) );
    }
    else if ( name == "varGcContent" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( argument ).getValue();
        
        double var_gc = this->dag_node->getValue().varGcContent( excl );
        
        return new RevVariable( new Probability(var_gc) );
    }
    else if ( name == "varGcContentByCodonPosition" )
    {
        found = true;
        
        const RevObject& argument = args[0].getVariable()->getRevObject();
        size_t n = size_t( static_cast<const Natural&>( argument ).getValue() );
        
        const RevObject& excl_argument = args[1].getVariable()->getRevObject();
        bool excl = static_cast<const RlBoolean&>( excl_argument ).getValue();
        
        double var_gc = this->dag_node->getValue().varGcContentByCodon( n, excl );
        
        return new RevVariable( new Probability(var_gc) );
    }
    
    return ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>::executeMethod( name, args, found );
}


/* Get Rev type of object */
const std::string& AbstractHomologousDiscreteCharacterData::getClassType(void)
{
    
    static std::string rev_type = "AbstractHomologousDiscreteCharacterData";
    
    return rev_type;
}

/* Get class type spec describing type of object */
const TypeSpec& AbstractHomologousDiscreteCharacterData::getClassTypeSpec(void)
{
    
    static TypeSpec rev_type_spec = TypeSpec( getClassType(), new TypeSpec( ModelObject<RevBayesCore::AbstractHomologousDiscreteCharacterData>::getClassTypeSpec() ) );
    
    return rev_type_spec;
}


/** Get type spec */
const TypeSpec& AbstractHomologousDiscreteCharacterData::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}


void AbstractHomologousDiscreteCharacterData::initMethods( void )
{
    
        // add the DAG node member methods
        // note that this is a sage case because all DAG nodes are member objects
    if ( dag_node != NULL )
    {
        const MethodTable &dagMethods = dynamic_cast<RevMemberObject*>( dag_node )->getMethods();
        methods.insertInheritedMethods( dagMethods );
    }
    
        // insert the character data specific methods
    MethodTable charDataMethods = getCharacterDataMethods();
    methods.insertInheritedMethods( charDataMethods );
    
    ArgumentRules* chartypeArgRules                 = new ArgumentRules();
    ArgumentRules* compStateFreqArgRules            = new ArgumentRules();
    ArgumentRules* compMultiLikeArgRules            = new ArgumentRules();
    ArgumentRules* empiricalBaseArgRules            = new ArgumentRules();
    ArgumentRules* ishomologousArgRules             = new ArgumentRules();
    ArgumentRules* invSitesArgRules                 = new ArgumentRules();
    ArgumentRules* setCodonPartitionArgRules        = new ArgumentRules();
    ArgumentRules* setCodonPartitionArgRules2       = new ArgumentRules();
    ArgumentRules* setNumStatesPartitionArgRules    = new ArgumentRules();
    ArgumentRules* squareBracketArgRules            = new ArgumentRules();
    
    ArgumentRules* maxGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* maxInvariableBlockLengthArgRules     = new ArgumentRules();
    ArgumentRules* maxVariableBlockLengthArgRules       = new ArgumentRules();
    ArgumentRules* minGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* maxPairwiseDifferenceArgRules        = new ArgumentRules();
    ArgumentRules* minPairwiseDifferenceArgRules        = new ArgumentRules();
    ArgumentRules* getPairwiseDifferenceArgRules        = new ArgumentRules();
    ArgumentRules* meanGcContentArgRules                = new ArgumentRules();
    ArgumentRules* meanGcContentByCodonPositionArgRules = new ArgumentRules();
    ArgumentRules* numInvariableBlocksArgRules          = new ArgumentRules();
    ArgumentRules* numTaxaMissingSequenceArgRules       = new ArgumentRules();
    ArgumentRules* varGcContentArgRules                 = new ArgumentRules();
    ArgumentRules* varGcContentByCodonPositionArgRules  = new ArgumentRules();
    
    ArgumentRules* translateCharactersArgRules          = new ArgumentRules();
    ArgumentRules* expandCharactersArgRules             = new ArgumentRules();
    ArgumentRules* getStateDescriptionsArgRules         = new ArgumentRules();
    
    
    setCodonPartitionArgRules->push_back(       new ArgumentRule("",        Natural::getClassTypeSpec()              , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    setCodonPartitionArgRules2->push_back(      new ArgumentRule("",        ModelVector<Natural>::getClassTypeSpec() , "The indicies of the codon positions.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    setNumStatesPartitionArgRules->push_back(   new ArgumentRule("",        Natural::getClassTypeSpec()              , "The number of states in this partition.", ArgumentRule::BY_VALUE, ArgumentRule::ANY ) );
    squareBracketArgRules->push_back(           new ArgumentRule( "index" , Natural::getClassTypeSpec()              , "The index of the taxon.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    
    expandCharactersArgRules->push_back(                new ArgumentRule( "factor"           , Natural::getClassTypeSpec()            , "The factor by which the state space is expanded.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    invSitesArgRules->push_back(                        new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxInvariableBlockLengthArgRules->push_back(        new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxVariableBlockLengthArgRules->push_back(          new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    minGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    maxPairwiseDifferenceArgRules->push_back(           new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    minPairwiseDifferenceArgRules->push_back(           new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    getPairwiseDifferenceArgRules->push_back(           new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    meanGcContentArgRules->push_back(                   new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    meanGcContentByCodonPositionArgRules->push_back(    new ArgumentRule( "index" , Natural::getClassTypeSpec()          , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    meanGcContentByCodonPositionArgRules->push_back(    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    numInvariableBlocksArgRules->push_back(             new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    numTaxaMissingSequenceArgRules->push_back(          new ArgumentRule( "x" ,     Probability::getClassTypeSpec()          , "The percentage/threshold for the missing sequence.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    translateCharactersArgRules->push_back(             new ArgumentRule( "type" ,     RlString::getClassTypeSpec()          , "The character type into which we want to translate.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    varGcContentArgRules->push_back(                    new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    varGcContentByCodonPositionArgRules->push_back(     new ArgumentRule( "index" , Natural::getClassTypeSpec()          , "The index of the codon position.", ArgumentRule::BY_VALUE, ArgumentRule::ANY  ) );
    varGcContentByCodonPositionArgRules->push_back(     new ArgumentRule( "excludeAmbiguous" , RlBoolean::getClassTypeSpec()          , "Should we exclude ambiguous and missing characters?", ArgumentRule::BY_VALUE, ArgumentRule::ANY, new RlBoolean( false )  ) );
    
    
    methods.addFunction( new MemberProcedure( "chartype",                               RlString::getClassTypeSpec(),       chartypeArgRules                ) );
    methods.addFunction( new MemberProcedure( "computeStateFrequencies",                MatrixReal::getClassTypeSpec(),     compStateFreqArgRules           ) );
    methods.addFunction( new MemberProcedure( "computeMultinomialProfileLikelihood",    Real::getClassTypeSpec(),           compMultiLikeArgRules           ) );
    methods.addFunction( new MemberProcedure( "setCodonPartition",                      RlUtils::Void,                      setCodonPartitionArgRules       ) );
    methods.addFunction( new MemberProcedure( "setCodonPartition",                      RlUtils::Void,                      setCodonPartitionArgRules2      ) );
    methods.addFunction( new MemberProcedure( "setNumStatesPartition",                  RlUtils::Void,                      setNumStatesPartitionArgRules   ) );
    methods.addFunction( new MemberProcedure( "isHomologous",                           RlBoolean::getClassTypeSpec(),      ishomologousArgRules            ) );
    methods.addFunction( new MemberProcedure( "expandCharacters",                       AbstractHomologousDiscreteCharacterData::getClassTypeSpec(),        expandCharactersArgRules         ) );
    methods.addFunction( new MemberProcedure( "getEmpiricalBaseFrequencies",            Simplex::getClassTypeSpec(),        empiricalBaseArgRules           ) );
    methods.addFunction( new MemberProcedure( "getNumInvariantSites",                   Natural::getClassTypeSpec(),        invSitesArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxGcContent",                           Probability::getClassTypeSpec(),    maxGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxInvariableBlockLength",               Natural::getClassTypeSpec(),        maxInvariableBlockLengthArgRules    ) );
    methods.addFunction( new MemberProcedure( "maxVariableBlockLength",                 Natural::getClassTypeSpec(),        maxVariableBlockLengthArgRules      ) );
    methods.addFunction( new MemberProcedure( "minGcContent",                           Probability::getClassTypeSpec(),    minGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "maxPairwiseDifference",                  Natural::getClassTypeSpec(),        maxPairwiseDifferenceArgRules       ) );
    methods.addFunction( new MemberProcedure( "minPairwiseDifference",                  Natural::getClassTypeSpec(),        minPairwiseDifferenceArgRules       ) );
    methods.addFunction( new MemberProcedure( "getPairwiseDifference",                  DistanceMatrix::getClassTypeSpec(), getPairwiseDifferenceArgRules       ) );
    methods.addFunction( new MemberProcedure( "meanGcContent",                          Probability::getClassTypeSpec(),    meanGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "meanGcContentByCodonPosition",           Probability::getClassTypeSpec(),    meanGcContentByCodonPositionArgRules                ) );
    methods.addFunction( new MemberProcedure( "numInvariableBlocks",                    Natural::getClassTypeSpec(),        numInvariableBlocksArgRules         ) );
    methods.addFunction( new MemberProcedure( "numTaxaMissingSequence",                 Natural::getClassTypeSpec(),        numTaxaMissingSequenceArgRules         ) );
    methods.addFunction( new MemberProcedure( "getStateDescriptions",                   ModelVector<RlString>::getClassTypeSpec(), getStateDescriptionsArgRules ) );
    methods.addFunction( new MemberProcedure( "translateCharacters",                    AbstractHomologousDiscreteCharacterData::getClassTypeSpec(),        translateCharactersArgRules         ) );
    methods.addFunction( new MemberProcedure( "varGcContent",                           Probability::getClassTypeSpec(),    varGcContentArgRules                ) );
    methods.addFunction( new MemberProcedure( "varGcContentByCodonPosition",            Probability::getClassTypeSpec(),    varGcContentByCodonPositionArgRules                ) );
    methods.addFunction( new MemberProcedure( "[]",                                     AbstractDiscreteTaxonData::getClassTypeSpec(), squareBracketArgRules) );
    
}


