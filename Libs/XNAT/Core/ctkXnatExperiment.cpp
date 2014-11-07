/*=============================================================================

  Library: XNAT/Core

  Copyright (c) University College London,
    Centre for Medical Image Computing

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkXnatExperiment.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSubject.h"
#include "ctkXnatScan.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatAssessor.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatAssessorFolder.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <QDebug>

//----------------------------------------------------------------------------
class ctkXnatExperimentPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatExperimentPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }

//  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatExperiment::ctkXnatExperiment(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatExperimentPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatExperiment::~ctkXnatExperiment()
{
}


// --------------------------------------------------------------------------
QList<ctkXnatScan*> ctkXnatExperiment::scans() const
{
  QList<ctkXnatScan*> result;

  ctkXnatScanFolder* folder = NULL;
  
  foreach(ctkXnatObject* obj, this->children())
  {
    ctkXnatScanFolder* o = dynamic_cast<ctkXnatScanFolder*>(obj);
    if (o)
    {
      folder = o;
      break;
    }
  }

  if (folder)
  {
    folder->fetch();
    foreach(ctkXnatObject* obj, folder->children())
    {
      ctkXnatScan* o = dynamic_cast<ctkXnatScan*>(obj);
      if (o)
	result.push_back (o);
    }
  }
  
  return result;
}


// --------------------------------------------------------------------------
QList<ctkXnatAssessor*> ctkXnatExperiment::assessors() const
{
  QList<ctkXnatAssessor*> result;

  ctkXnatAssessorFolder* folder = NULL;
  
  foreach(ctkXnatObject* obj, this->children())
  {
    ctkXnatAssessorFolder* o = dynamic_cast<ctkXnatAssessorFolder*>(obj);
    if (o)
    {
      folder = o;
      break;
    }
  }

  if (folder)
  {
    folder->fetch();
    foreach(ctkXnatObject* obj, folder->children())
    {
      ctkXnatAssessor* o = dynamic_cast<ctkXnatAssessor*>(obj);
      if (o)
	result.push_back (o);
    }
  }
  
  return result;
}


// --------------------------------------------------------------------------
QList<ctkXnatReconstruction*> ctkXnatExperiment::reconstructions() const
{
  QList<ctkXnatReconstruction*> result;

  ctkXnatReconstructionFolder* folder = NULL;
  
  foreach(ctkXnatObject* obj, this->children())
  {
    ctkXnatReconstructionFolder* o = dynamic_cast<ctkXnatReconstructionFolder*>(obj);
    if (o)
    {
      folder = o;
      break;
    }
  }

  if (folder)
  {
    folder->fetch();
    foreach(ctkXnatObject* obj, folder->children())
    {
      ctkXnatReconstruction* o = dynamic_cast<ctkXnatReconstruction*>(obj);
      if (o)
	result.push_back (o);
    }
  }
  
  return result;
}

//----------------------------------------------------------------------------
QString ctkXnatExperiment::resourceUri() const
{
  return QString("%1/experiments/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::fetchImpl()
{
  QString scansUri = this->resourceUri() + "/scans";
  ctkXnatSession* const session = this->session();
  QUuid scansQueryId = session->httpGet(scansUri);

  QList<ctkXnatObject*> scans;
  
  try
  {
    scans = session->httpResults(scansQueryId,
				 ctkXnatDefaultSchemaTypes::XSI_SCAN);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }

  if (!scans.isEmpty())
  {
    ctkXnatScanFolder* scanFolder = new ctkXnatScanFolder();
    this->add(scanFolder);
  }

  QString reconstructionsUri = this->resourceUri() + "/reconstructions";
  QUuid reconstructionsQueryId = session->httpGet(reconstructionsUri);

  QList<ctkXnatObject*> reconstructions;
  try
  {
    reconstructions = session->httpResults(reconstructionsQueryId,
					   ctkXnatDefaultSchemaTypes::XSI_RECONSTRUCTION);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }
  
  if (!reconstructions.isEmpty())
  {
    ctkXnatReconstructionFolder* reconstructionFolder = new ctkXnatReconstructionFolder();
    this->add(reconstructionFolder);
  }

  QString assessorsUri = this->resourceUri() + "/assessors";
  QUuid assessorsQueryId = session->httpGet(assessorsUri);
  
  QList<ctkXnatObject*> assessors;
  
  try
  {
    assessors = session->httpResults(assessorsQueryId,
				     ctkXnatDefaultSchemaTypes::XSI_ASSESSOR);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }

  if (!assessors.isEmpty())
  {
    ctkXnatAssessorFolder* assessorFolder = new ctkXnatAssessorFolder(this);
    this->add(assessorFolder);
  }

  this->fetchResources();

}
