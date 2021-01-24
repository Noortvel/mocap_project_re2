using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEditor;
using UnityEngine;

public class RenderTexturesSaver : MonoBehaviour
{
    [SerializeField]
    private Camera cameraR = null;
    [SerializeField]
    private Camera cameraL = null;

    [SerializeField]
    private Transform cubesParent = null;

    private const string baseOutImagesPath = "Assets/0OutImages/";

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.G))
        {
            if (cameraR != null && cameraL != null)
            {
                SaveRTToFile(cameraR.targetTexture, "ImageR");
                SaveRTToFile(cameraL.targetTexture, "ImageL");
                Debug.Log("Saved");
            }
        }
    }
    private void SaveDistancesInfo()
    {
        //StringBuilder stringBuilder = new StringBuilder();
        //for(int i = 0; i < cubesParent.childCount; i++)
        //{

        //}
    }
    private void SaveRTToFile(RenderTexture rt, string name)
    {

        RenderTexture.active = rt;
        Texture2D tex = new Texture2D(rt.width, rt.height, TextureFormat.RGB24, false);
        tex.ReadPixels(new Rect(0, 0, rt.width, rt.height), 0, 0);
        RenderTexture.active = null;

        byte[] bytes;
        bytes = tex.EncodeToPNG();

        string path = $"{baseOutImagesPath}{name}.png";
        System.IO.File.WriteAllBytes(path, bytes);
        AssetDatabase.ImportAsset(path);
    }
}
