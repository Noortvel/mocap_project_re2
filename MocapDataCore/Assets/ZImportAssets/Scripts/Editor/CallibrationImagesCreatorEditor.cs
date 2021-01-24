using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(CallibrationImagesCreator))]
public class CallibrationImagesCreatorEditor : Editor
{
    private CallibrationImagesCreator callibrationImagesCreator;
    private void OnEnable()
    {
        callibrationImagesCreator = target as CallibrationImagesCreator;
    }
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        if (GUILayout.Button("Save"))
        {
            //callibrationImagesCreator.Save();
            var routine = callibrationImagesCreator.SaveWaiter();
            callibrationImagesCreator.StartCoroutine(routine);
        }
    }
}
