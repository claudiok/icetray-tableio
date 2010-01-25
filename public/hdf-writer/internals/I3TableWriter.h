/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> Last changed by: $LastChangedBy$
 */

#ifndef	I3TABLEWRITER_H_INCLUDED
#define I3TABLEWRITER_H_INCLUDED

#include "icetray/IcetrayFwd.h"

#include <string>

#include "hdf-writer/internals/I3Converter.h"

I3_FORWARD_DECLARATION(I3TableService);
I3_FORWARD_DECLARATION(I3Table);

class I3TableWriter {
    public:
        I3TableWriter(I3TableServicePtr service);
        virtual ~I3TableWriter();
        
        // register one specific object. if type and converter are empty the writer 
        // should figure out appropriate values
        void AddObject(std::string name, std::string tableName, 
                       std::string type, std::string converter, 
                       const StringPairVector& params);
        
        // write all objects with this type
        void AddType(std::string typeName, std::string converter,
                     const StringPairVector& params);

        void Setup();

        void Convert(I3FrameConstPtr frame);
        
        void Finish();

    protected:
        I3TablePtr ConnectTable(std::string tableName, 
                                const I3TableRowDescription& description);
        void DisconnectTable(I3TablePtr& table);
        
        
        // group components that belong togehter
        struct TableBundle {
            std::string objectType;
            I3ConverterPtr converter;
            I3TablePtr table;
        };

        I3TableServicePtr service_;
        std::map<std::string, TableBundle> tables_;
        std::map<std::string, I3ConverterPtr> converters_;
         
        // wanted lists 
        std::vector<std::string> wantedNames_;
        std::vector<std::string> wantedTypes_;
        I3FrameConstPtr currentFrame_;
        I3ConverterPtr ticConverter_;

    private:
        I3TableWriter();
        I3TableWriter(const I3TableWriter& rhs);


};

I3_POINTER_TYPEDEFS( I3TableWriter );

#endif
