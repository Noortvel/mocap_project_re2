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

    private class NamePosition
    {
        public NamePosition(string name, Vector3 position)
        {
            Name = name;
            Position = position;
        }

        public string Name;
        public Vector3 Position;
    }

    public void SaveObjectDistances()
    {
        var list = new List<NamePosition>();
        for (int i = 0; i < parentObject.transform.childCount; i++)
        {
            var child = parentObject.transform.GetChild(i);
            if (child.gameObject.activeSelf)
            {
                list.Add(new NamePosition(child.gameObject.name, child.localPosition));
            }
        }
        string str = JsonUtiltyExtensions.FromList(list);
        using (var steam = new StreamWriter(pathName, false))
        {
            steam.WriteLine(str);
        }
    }
}
