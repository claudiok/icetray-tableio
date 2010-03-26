/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/converter/I3TriggerHierarchyConverter.h"

size_t I3TriggerHierarchyConverter::GetNumberOfRows(const I3TriggerHierarchy& triggers) {
    return triggers.size();
}

I3TableRowDescriptionPtr I3TriggerHierarchyConverter::CreateDescription(const I3TriggerHierarchy& triggers) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->isMultiRow_ = true;
    desc->AddField<tableio_size_t>("depth", "",     "depth in the tree");
    desc->AddField<double>("time",          "ns",   "time at which the trigger was issued");
    desc->AddField<double>("length",        "ns",   "duration of triggered readout window");
    desc->AddField<bool>("fired",           "bool", "true, if trigger fired (used for simulations)");
    
    // TriggerKey
    MAKE_ENUM_VECTOR(sourceID,TriggerKey,TriggerKey::SourceID,TRIGGERKEY_H_TriggerKey_SourceID);
    MAKE_ENUM_VECTOR(typeID,TriggerKey,TriggerKey::TypeID,TRIGGERKEY_H_TriggerKey_TypeID);
    MAKE_ENUM_VECTOR(subtypeID,TriggerKey,TriggerKey::SubtypeID,TRIGGERKEY_H_TriggerKey_SubtypeID);
    
    desc->AddEnumField<TriggerKey::SourceID>("source_id", sourceID, "",
                                             "Enumeration describing what 'subdetector' issued a trigger");
    desc->AddEnumField<TriggerKey::TypeID>("type_id", typeID, "",
                                           "Enumeration describing what 'algorithm' issued a trigger");
    desc->AddEnumField<TriggerKey::SubtypeID>("subtype_id", subtypeID, "",
                                              "Enumeration describing how a software trigger was orginally 'configured' within the TWR DAQ trigger system");
    desc->AddField<int32_t>("config_id", "", "configID member of TriggerKey"); // FIXME: docs
    
    return desc;
};

size_t I3TriggerHierarchyConverter::FillRows(const I3TriggerHierarchy& triggers, I3TableRowPtr rows) {
    size_t nrows = rows->GetNumberOfRows();
    size_t currentRow = rows->GetCurrentRow();
    assert( (nrows - currentRow) >= static_cast<size_t>(triggers.size()));

    I3TriggerHierarchy::iterator trigger;

    size_t i_row = currentRow;
    for (trigger = triggers.begin(); trigger != triggers.end(); ++trigger) {
        rows->SetCurrentRow(i_row);
        tableio_size_t depth = triggers.depth(trigger);

        rows->Set<tableio_size_t>("depth", depth);
        
        rows->Set<double>("time", trigger->GetTriggerTime());
        rows->Set<double>("length", trigger->GetTriggerLength());
        rows->Set<bool>("fired", trigger->GetTriggerFired());

        const TriggerKey& key = trigger->GetTriggerKey();

        rows->Set<TriggerKey::SourceID> ("source_id",   key.GetSource());
        rows->Set<TriggerKey::TypeID> ("type_id",       key.GetType());
        rows->Set<TriggerKey::SubtypeID> ("subtype_id", key.GetSubtype());

        i_row++; 
    }

    return i_row - currentRow;
};

I3_CONVERTER(I3TriggerHierarchyConverter, I3TriggerHierarchy);

