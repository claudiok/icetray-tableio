# -*- coding: utf-8 -*-
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id$
# 
# @version $Revision$
# @date $LastChangedDate$
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$
# 

class I3ConverterRegistry(object):
    """A mapping of object type->I3Converter. 
    
    This should be considered an implementation detail and subject to change without notice."""
    registry = dict()
    defaults = dict()
    def register(cls,converter,object_type=None,is_default=False):
        if object_type is None:
            object_type = converter.booked
        if object_type not in cls.registry:
            cls.registry[object_type] = []
        converters = cls.registry[object_type]
        converters.append(converter)
        if is_default:
            if object_type in cls.defaults:
                 raise TypeError("Attempted to register '%s' as the default converter for '%s', but '%s' has already been registered as default!" % (converter, object_type, cls.defaults[object_type]))
            cls.defaults[object_type] = converter
    register = classmethod(register)
    def update(cls,registry_dict):
        for conv,targets in registry_dict.items():
            if conv in cls.registry:
                cls.registry[conv] += targets
            else:
                cls.registry[conv] = targets
    update = classmethod(update)


