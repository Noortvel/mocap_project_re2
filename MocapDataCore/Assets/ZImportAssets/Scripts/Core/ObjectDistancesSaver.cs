using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class ObjectDistancesSaver : MonoBehaviour
{
    [SerializeField]
    private string pathName = "ObjectsDistances.json";

    [SerializeField]
    private Transform cameraPosition;

    [SerializeField]
    private Transform parentObject;

    public void SaveObjectDistances()
    {
        var list = new List<(string Name, Vector3 Position)>();
        for (int i = 0; i < parentObject.transform.childCount; i++)
        {
            var child = parentObject.transform.GetChild(i);
            if (child.gameObject.activeSelf)
            {
                list.Add((child.gameObject.name, child.localPosition));
            }
        }
        string str = JsonUtiltyExtensions.FromList(list);
        using (var steam = new StreamWriter(pathName, false))
        {
            steam.WriteLine(str);
        }
    }
}
