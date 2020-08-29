using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(VibManager))]
public class VibManagerEditor : Editor
{
    VibManager vibManager;

    private void OnEnable()
    {
        vibManager = (VibManager)target;
    }

    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();

    }



}
