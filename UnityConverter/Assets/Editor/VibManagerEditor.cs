using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

[CustomEditor(typeof(VibManager))]
public class VibManagerEditor : Editor
{

    public class VibDataContent
    {
        public string HeaderDesc;
        public string RawBase64;
        public string RawData;
        public int Size;
        public int Version;
    }


    VibManager vibManager;
    VibKey selectedKey = (VibKey) 0;

    private void OnEnable()
    {
        vibManager = (VibManager)target;

    }

    public override void OnInspectorGUI()
    {
        EditorGUILayout.Space();
        EditorGUILayout.Space();

        GUILayout.BeginHorizontal();
            selectedKey = (VibKey) EditorGUILayout.EnumPopup("VibKey", selectedKey);

            if(GUILayout.Button("Test"))
            {
                TestVibration();
            }
        GUILayout.EndHorizontal();

        EditorGUILayout.Space();

        EditorGUILayout.HelpBox("Test works on runtime only", MessageType.Warning);
        EditorGUILayout.HelpBox("Usage : OVRHaptics.LeftChannel.Mix(VibManager.vibDictionary[Vibkey.VibSample1]);", MessageType.Info);

        EditorGUILayout.Space();
        EditorGUILayout.Space();

        if(GUILayout.Button("Refresh Vibkey"))
        {
            EnumerizeKeys();
        }

    }

    private void EnumerizeKeys()
    {
        TextAsset[] _vibDataFiles = Resources.LoadAll<TextAsset>("VibData");

        string enumTitle = "VibKey";                   
        List<string> enumEntries = new List<string>();                  //add vibdata names to string list to enumerize them
        for(int _i = 0 ; _i < _vibDataFiles.Length ; _i ++)
        {        
            enumEntries.Add(_vibDataFiles[_i].name);
        }

        using (StreamWriter streamWriter = new StreamWriter("Assets/BravesVibUnityConverter/Scripts/" + enumTitle + ".cs"))
        {
            streamWriter.WriteLine("public enum " + enumTitle);
            streamWriter.WriteLine("{");
            for(int _i = 0 ; _i < enumEntries.Count ; _i++)
            {
                streamWriter.WriteLine("\t" + enumEntries[_i] + ",");
            }
            streamWriter.WriteLine("}");
        }

        UnityEditor.AssetDatabase.Refresh();
    }

    private void TestVibration()
    {
        OVRHaptics.RightChannel.Mix(VibManager.vibDictionary[selectedKey]);
        OVRHaptics.LeftChannel.Mix(VibManager.vibDictionary[selectedKey]);
    }


}
