/*
 *  main.cpp
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "TagHandler.h"
#include "SWFParser.h"


/*
 // 2 passes :
 // - 1. parsing to generate the object tree. (Know the total number of objects)
 // - 2. iterate the tree. (BFS/DFS)
 SWFParser parser;
 if (parser.open("filename.swf")) {
    VObject = parser.parse(); // parse the all VObject tree, return root VObject.
    parser.BFSIterate(); ? // get every object iterated.
    parser.DFSIterate(); ?
    parser.close();
 };
 parser.close();
 */ 


typedef std::vector<VObject *> VList;

VList *nextList;
int depth = 0;


void obj_traverse(VObject *vobj);

void pushToNextList(VObject *vobj)
{
    if (!nextList) {
        nextList = new VList();
    }
    nextList->push_back(vobj);
}

void traverse_properties(PropertyList *plist) 
{
    for (std::vector<Property>::iterator it = plist->begin(); it != plist->end(); it++) {
        //char *pName = 0, *pValue = 0;   // This should be the (name, value) pair for the row of a Table.
        // it = Property *;
        for (int j = 0; j < depth; j++)
            fprintf(stderr, "\t");

        fprintf(stderr, "name = %s, ", it->name.c_str());
        
        VObject *value = it->value;
        if (value->isValue()) {
            fprintf(stderr, " value = ");
            switch (value->getType()) {
                case NullType:
                    fprintf(stderr, "Null\n");
                    break;
                case StringType:
                    fprintf(stderr, "String(%s)\n", value->asString());
                    break;
                case IntType:
                    fprintf(stderr, "Int(%d)\n", value->asInt());
                    break;
                case UIntType:
                    fprintf(stderr, "UInt(%d)\n", value->asUInt());
                    break;
                case DoubleType:
                    fprintf(stderr, "Double(%f)\n", value->asDouble());
                    break;
                default:
                    fprintf(stderr, "Error: Non-defined Type\n");
                    abort();
                    break;
            }
        }
        
        if (value->isObject()) {
            fprintf(stderr, "Object : %s\n", value->getTypeInfo());
            // get value->typeInfo();
            // should add
            // push into nextList
            pushToNextList(value);
        }
        
        if (value->isArray()) {
            fprintf(stderr, "is Array\n");
            obj_traverse(value);
        }
    }
}

// VObject with primitive type value is not called by this function.
// They are handled inside the function.
void obj_traverse(VObject *vobj)
{
    if (vobj->isArray()) {
        // IS Array allowed to hold another array directly?
        //fprintf(stderr, "is array, somthing wrong?\n");
        PropertyList *plist = vobj->getPropertyList();
        for (std::vector<Property>::iterator it = plist->begin(); it != plist->end(); it++) {
            //char *pName = 0, *pValue = 0;   // This should be the (name, value) pair for the row of a Table.
            // it = Property *;
            for (int j = 0; j < depth; j++)
                fprintf(stderr, "\t");

            fprintf(stderr, "[%s] = ", it->name.c_str());
            
            VObject *value = it->value;
            if (value->isValue()) {
                switch (value->getType()) {
                    case NullType:
                        fprintf(stderr, "Null\n");
                        break;
                    case StringType:
                        fprintf(stderr, "String(%s)\n", value->asString());
                        break;
                    case IntType:
                        fprintf(stderr, "Int(%d)\n", value->asInt());
                        break;
                    case UIntType:
                        fprintf(stderr, "UInt(%d)\n", value->asUInt());
                        break;
                    case DoubleType:
                        fprintf(stderr, "Double(%f)\n", value->asDouble());
                        break;
                    default:
                        fprintf(stderr, "Error: Non-defined Type\n");
                        abort();
                        break;
                }
            }
            
            if (value->isObject()) {
                fprintf(stderr, "Object : %s\n", value->getTypeInfo());
                // get value->typeInfo();
                // should add
                // push into nextList
                pushToNextList(value);
            }
            
            if (value->isArray()) {
                fprintf(stderr, "is Array : (Something wrong? array can hold another array?)\n");
                abort();
                //obj_traverse(value);
            }
        }
    }
    
    if (vobj->isObject()) {
        // get the type info as the header of the Table
        const char *header = vobj->getTypeInfo();
        
        for (int j = 0; j < depth; j++)
            fprintf(stderr, "\t");
        
        fprintf(stderr, "Header = %s\n", header);
        // get propertyList
        PropertyList *plist = vobj->getPropertyList();
        traverse_properties(plist);
    }
    // if this is an object.
    // 1. get properties.
    // if this is an array.
    // else is primitive types.
    // return value?
}

void BFSTraverse(VObject *root)
{	
    //////////////////////////
    // BFS Traverse
    
    VList *currentList = new VList();
    
    // push root into currentList
    currentList->push_back(root);
    
    while (currentList) {
        nextList = NULL;
        // Traverse each VObject in the VList.
        for(std::vector<VObject *>::iterator vl_it = currentList->begin(); vl_it != currentList->end(); vl_it++) {
            // current VObject = *vl_it;
            obj_traverse(*vl_it);
        }
        // release currentList since it's parsed.
        delete currentList;
        currentList = NULL;
        if (nextList) {
            // push nextList to depthList;
            //depthList.push_back(nextList);
            currentList = nextList;
        }
        depth++;
    }
    
    // END of BFS Traverse
    //////////////////////////
}

int main(int argc, char *argv[])
{
	
	SWFParser parser;
    VObject *root;
    
    root = parser.parse("/Users/rintarou/Projects/swfparser/trunk/clips/t.swf");
    fprintf(stderr, "%d\n", root->numVObjects());
    BFSTraverse(root);

    /*
    if (argc < 2) {
        fprintf(stderr, "usage: %s file.swf\n", argv[0]);
        return TRUE;
    }
    root = parser.parse(argv[1]);
    
     if (root) {
     std::string str;
     root->toJSON(str);
     fprintf(stdout, "%s\n", str.c_str());
     }
     */
    
	return TRUE;
}

