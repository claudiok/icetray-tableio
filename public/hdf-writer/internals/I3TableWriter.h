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
                       std::string type, std::string converter);

        // register one specific object, lazily. if type and converter are empty the writer 
        // should figure out appropriate values
        void AddObject(std::string name, std::string tableName, 
                       std::string type, I3ConverterPtr converter, I3FrameObjectConstPtr obj);
        
    	struct TableSpec {
			std::string tableName;
			I3ConverterPtr converter;
			TableSpec(const std::string name,I3ConverterPtr conv) : tableName(name),converter(conv) {};
			TableSpec(const std::string name) : tableName(name),converter() {};
			TableSpec(I3ConverterPtr conv) : tableName(),converter(conv) {};
			TableSpec() : tableName(),converter() {};
		};
        
        // add object to wanted list
        void AddObject(std::string typeName, TableSpec spec);
        // write all objects with this type
        void AddType(std::string typeName, TableSpec spec);
        
        void AddConverter(std::string typeName, I3ConverterPtr converter);

        void Setup();

        void Convert(I3FramePtr frame);
        
        void Finish();




    protected:
        I3TablePtr ConnectTable(std::string tableName, 
                                const I3TableRowDescription& description);
        void DisconnectTable(I3TablePtr& table);
        
        
        // group components that belong together
        struct TableBundle {
            std::string objectType;
            I3ConverterPtr converter;
            I3TablePtr table;
        };


        I3TableServicePtr service_;
        std::map<std::string, std::vector<TableBundle> > tables_;
        std::map<std::string, I3ConverterPtr> converters_;
         
		typedef std::map<std::string, std::vector<TableSpec> > tablespec_map;
        // configuration lists and maps
		tablespec_map wantedNames_;
		tablespec_map wantedTypes_; 
        // std::vector<std::string> wantedNames_;
        // std::vector<std::string> wantedTypes_;
        std::map<std::string,std::string> objNameToTableName_;
        std::map<std::string,std::string> typeNameToConverterName_;
        I3FrameConstPtr currentFrame_;
        I3ConverterPtr ticConverter_;

    private:
        I3TableWriter();
        I3TableWriter(const I3TableWriter& rhs);

    SET_LOGGER("I3TableWriter");
};

I3_POINTER_TYPEDEFS( I3TableWriter );

#endif
