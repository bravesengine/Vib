using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

public class EnumGenerator : MonoBehaviour
{
     [MenuItem( "Tools/EnumGenerator" )]
     public static void Go()
     {
         string enumName = "MyEnum";
         string[] enumEntries = { "Foo", "Goo", "Hoo" };
         string filePathAndName = "Assets/Scripts/Enums/" + enumName + ".cs"; //The folder Scripts/Enums/ is expected to exist
 
         using ( StreamWriter streamWriter = new StreamWriter( filePathAndName ) )
         {
             streamWriter.WriteLine( "public enum " + enumName );
             streamWriter.WriteLine( "{" );
             for( int i = 0; i < enumEntries.Length; i++ )
             {
                 streamWriter.WriteLine( "\t" + enumEntries[i] + "," );
             }
             streamWriter.WriteLine( "}" );
         }
         AssetDatabase.Refresh();
     }
}
