using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CallibrationImagesCreator : MonoBehaviour
{
    [SerializeField]
    private RenderTexture captureTarget = null;
    [SerializeField]
    private string imagePrefix = null;

    private const string baseOutImagesPath = "Assets/Callibrations/";

    [ExecuteInEditMode]
    public void Save()
    {
        for(int i = 0; i < transform.childCount; i++)
        {
            transform.GetChild(i).gameObject.SetActive(false);
        }
        for (int i = 0; i < transform.childCount; i++)
        {
            var child = transform.GetChild(i);
            child.gameObject.SetActive(true);

            //RenderTextureImageSaver.Save(captureTarget, $"{baseOutImagesPath}imagePrefix-{i}.png");
            child.gameObject.SetActive(false);
        }
    }
    private float _waitTime = 0.1f;
    public IEnumerator SaveWaiter()
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            transform.GetChild(i).gameObject.SetActive(false);
        }
        for (int i = 0; i < transform.childCount; i++)
        {
            var child = transform.GetChild(i);
            child.gameObject.SetActive(true);
            yield return new WaitForSeconds(_waitTime);
            RenderTextureImageSaver.Save(captureTarget, $"{baseOutImagesPath}Callibration-{imagePrefix}-{i}");
            child.gameObject.SetActive(false);
        }

    }
}
